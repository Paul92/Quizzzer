#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <pthread.h>

#define PORT 9898

#define READ_BUF_SIZE 1024

#define PASSWORD_SIZE             128    // Maxim dimension of password
#define USER_SIZE                 128    // Maxim dimensions of user
#define COMMAND_SIZE              128    // Size of command
#define SQL_COMMAND_MAX_SIZE      512    // Size of sql commands
#define MAX_LOGGED_PLAYERS        20     // The maxim number of players connected
#define NO_GAME_PLAYERS           2
#define NO_OF_ANSWERS             4
#define NO_QUESTIONS              2
#define MAX_QUESTION_LENGTH       200
#define MAX_ANSWER_LENGTH         200
#define ANSWER_TIMEOUT            15
#define MAX_GAME_MASTERS          MAX_LOGGED_PLAYERS / NO_GAME_PLAYERS
#define MAX_QUESTION_COMMAND_SIZE 2014
#define MAX_SCOREBOARD_COMMAND_SIZE 2014

#define DATABASE_NAME             "../QuizzGame.db"

#define LOGIN_COMMAND             "login"
#define NEWUSER_COMMAND           "newuser"
#define LOGOUT_COMMAND            "logout"
#define QUIT_COMMAND              "quit"
#define GAME_STARTED_COMMAND      "game_started\n\0"
#define GAME_NEWQUESTION_COMMAND  "question"
#define GAME_SCOREBOARD_COMMAND   "scoreboard"
#define ANSWER_COMMAND            "answer"
#define GAME_END_COMMAND          "game_end\n\0"

void *game_master(void *arg);

struct Thread_data {
    int client_fd; // Descriptor returned by accept function
};
// Defining a structure which retains details about players
struct Player {
    int fd;
    char username[USER_SIZE];
    int loggedIn;
    struct Game *game;
    int correctAnswers;
};

// Defining a structure which retains details about game
struct Game {
    int questionCount;
    pthread_barrier_t barrier;
    struct Question *currentQuestion;
    pthread_t thread_id;
    int ended;
};

struct Game *newGame() {
    struct Game *newGame = malloc(sizeof(struct Game));
    newGame->questionCount = 0;
    // Init barrier with NO_GAME_PLAYERS threads for the games and 1 for the game master thread
    pthread_barrier_init(&(newGame->barrier), NULL, NO_GAME_PLAYERS + 1);
    newGame->ended = 0;
    return newGame;
}

void deleteGame(struct Game *game) {
    pthread_barrier_destroy(&(game->barrier));
    free(game);
}

// The correct answer is stored on the first position of answers
struct Question {
    char question[MAX_QUESTION_LENGTH];
    char answers[NO_OF_ANSWERS][MAX_ANSWER_LENGTH];
};

// NULL init by default
struct Player *players[MAX_LOGGED_PLAYERS];

pthread_mutex_t mutex;

int login_query(void *ret, int argc, char **argv, char **azColName) {
    int *retP = ret;
    *retP = argc != 0;
    return 0;
}

int register_query(void *NotUsed, int argc, char **argv, char **azColName) {
    printf("Received a register query with %d params\n", argc);
    return 0;
}

struct Question *newQuestion() {
    return malloc(sizeof(struct Question));
}

void deleteQuestion(struct Question *question) {
    free(question);
}

// For debugging purposes
void printQuestion(struct Question *question) {
    printf("Question: %s\n", question->question);
    for (int index = 0; index < NO_OF_ANSWERS; index++) {
        printf("Answer %d: %s\n", index, question->answers[index]);
    }
}

int retrieve_question_query(void *returnValue, int argc, char **argv, char **azColName) {
    // Check if there are enough parameters returned from the database, which represent the question id,
    // the question itself and the possible answers
    if (argc != NO_OF_ANSWERS + 2) {
        fprintf(stderr, "Wrong number of parameters received from database");
        exit(0);
    }

    struct Question *question = returnValue;
    strcpy(question->question, argv[1]);
    for (int index = 0; index < NO_OF_ANSWERS; index++) {
        strcpy(question->answers[index], argv[index + 2]);
    }
    return 0;
}

struct Question *getNewQuestion(sqlite3 *db) {
    struct Question *question = newQuestion();
    char *sql_command = "SELECT * FROM Questions ORDER BY RANDOM() LIMIT 1";
    char *zErrMsg = NULL;

    if (sqlite3_exec(db, sql_command, retrieve_question_query, question, &zErrMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    }

    return question;
}

sqlite3 *open_db() {
    sqlite3 *db;
    if (sqlite3_open(DATABASE_NAME, &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stdout, "Database opened successfully\n");
        return db;
    }
}

struct Player *newPlayer(int fd) {
    struct Player *player = malloc(sizeof(struct Player));

    player->fd       = fd;
    player->loggedIn = 0;
    player->game     = NULL;
    player->correctAnswers = 0;

    return player;
}

void deletePlayer(struct Player *player) {
    free(player);
}

enum LoginCodes {LOGIN_FAILED, LOGIN_SUCESSFUL, GAME_STARTED};

enum LoginCodes handle_login_command(sqlite3 *db, struct Player *player,
                                     char *username, char *password) {

    int client_fd = player->fd;

    char sql_command[SQL_COMMAND_MAX_SIZE];
    sprintf(sql_command, "SELECT * FROM Users WHERE "
                         "user = \"%s\" AND password = \"%s\";",
                         username, password);
    printf("Doing login\n");

    char *zErrMsg = NULL;

    int login_query_return = 0;
    if (sqlite3_exec(db, sql_command, login_query,
                     &login_query_return, &zErrMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    }
    // If username and password is not good, then will print an error message
    if (login_query_return == 0) {
        printf("Login failed.\n");
        write(client_fd, "FAILED\n", 8);
        return LOGIN_FAILED;
    } else {
        // Login succeeded
        printf("Login successful.\n");
        write(client_fd, "OK\n", 4);

        pthread_mutex_lock(&mutex);
        player->loggedIn = 1;
        strcpy(player->username, username);

        int noOfLoggedInPlayers = 0;
        for (int playerIndex = 0; playerIndex < MAX_LOGGED_PLAYERS; playerIndex++)
            if (players[playerIndex] != NULL && players[playerIndex]->game == NULL)
                noOfLoggedInPlayers += players[playerIndex]->loggedIn;

        pthread_mutex_unlock(&mutex);

        if (noOfLoggedInPlayers == NO_GAME_PLAYERS) {
            printf("Let the games begin!\n");
            struct Game *game = newGame();
            pthread_mutex_lock(&mutex);
            for (int playerIndex = 0; playerIndex < MAX_LOGGED_PLAYERS; playerIndex++)
                if (players[playerIndex] && players[playerIndex]->game == NULL && players[playerIndex]->loggedIn)
                    players[playerIndex]->game = game;
            pthread_mutex_unlock(&mutex);

            pthread_create(&(game->thread_id), NULL, game_master, game);

            return GAME_STARTED;
        }
    }
    return LOGIN_SUCESSFUL;
}

// Inserting new user in database
void handle_newuser_command(sqlite3 *db, struct Player *player, char *username, char *password) {
    char sql_command[SQL_COMMAND_MAX_SIZE];

    sprintf(sql_command, "INSERT INTO Users(user, password) "
                         "VALUES (\"%s\", \"%s\");",
                         username, password);
    char *zErrMsg = NULL;

    // If insertion has failed, then an error message will be printed
    if (sqlite3_exec(db, sql_command, register_query, NULL,
                     &zErrMsg) != SQLITE_OK) {
        write(player->fd, "FAILED\n", 8);
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "You are now registered!\n");
        write(player->fd, "OK\n", 4);
    }
}

void handle_logout_command(struct Player *player) {
    if (player->loggedIn) {
        printf("[server] The client with"
               "%d descriptor has logout.\n", player->fd);
        pthread_mutex_lock(&mutex);
        player->loggedIn = 0;
        pthread_mutex_unlock(&mutex);
        // TODO: REMOVE FROM ANY ACTIVE GAME
    } else {
        printf("[server] The client with %d descriptor"
                "is not logged in\n", player->fd);
    }
}

void handle_answer_command(struct Player *player, char *answer, int correctAnswer) {
    int answerId;
    sscanf(answer, "%d", &answerId);
    if (answerId == correctAnswer) {
        player->correctAnswers++;
        write(player->fd, "OK\n", 4);
    } else {
        write(player->fd, "WRONG\n", 7);
    }
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int *randomArray(int n) {
    int *arr = malloc(n * sizeof(int));
    for (int index = 0; index < n; index++)
        arr[index] = index;
    srand(time(NULL));
    // Warning: non uniform distribution
    for (int index = 0; index < n; index++) {
        int rnd = rand() % n;
        swap(arr + index, arr + rnd);
    }
    return arr;
}

void scoreUpdate(struct Game *game, int fd) {
    char newScoreboardCommand[MAX_SCOREBOARD_COMMAND_SIZE];
    strcpy(newScoreboardCommand, GAME_SCOREBOARD_COMMAND);
    for (int player = 0; player < MAX_LOGGED_PLAYERS; player++) {
        if (players[player] && players[player]->game == game) {
            sprintf(newScoreboardCommand, "%s|%s:%d", newScoreboardCommand,
                    players[player]->username, players[player]->correctAnswers);
        }
    }
    strcat(newScoreboardCommand, "\n");
    write(fd, newScoreboardCommand, strlen(newScoreboardCommand));
//    for (int player = 0; player < MAX_LOGGED_PLAYERS; player++) {
//        if (players[player] && players[player]->game == game) {
//            write(players[player]->fd, newScoreboardCommand,
//                  strlen(newScoreboardCommand));
//        }
//    }
}

void start_game_notice(struct Game *game) {
    sleep(1);
    for (int player = 0; player < MAX_LOGGED_PLAYERS; player++)
        if (players[player] && players[player]->game == game)
            write(players[player]->fd, GAME_STARTED_COMMAND, strlen(GAME_STARTED_COMMAND));
    sleep(1);
}

void *game_master(void *arg) {
    struct Game *game = arg;
    start_game_notice(game);

    sqlite3 *db = open_db();

    while (game->questionCount < NO_QUESTIONS) {
        pthread_barrier_wait(&(game->barrier));
        game->currentQuestion = getNewQuestion(db);
        pthread_barrier_wait(&(game->barrier));
        sleep(ANSWER_TIMEOUT);
        pthread_barrier_wait(&(game->barrier));
        deleteQuestion(game->currentQuestion);
        sleep(1);
        pthread_barrier_wait(&(game->barrier));
        game->questionCount++;
    }
    pthread_barrier_wait(&(game->barrier));
    printf("Game master ended");
    deleteGame(game);
    sqlite3_close(db);
    pthread_detach(pthread_self());
    return NULL;
}

void *treat(void *arg) {

    struct Player *currentPlayer = arg;

    sqlite3 *db = open_db();

    char buffer[READ_BUF_SIZE];
    int correctAnswer = 0;

    while (1) {
        // Warning: race condition ahead...
        if (currentPlayer->game != NULL) {
            if (currentPlayer->game->questionCount == NO_QUESTIONS) {
                printf("GAME END\n");
                sleep(1);
                write(currentPlayer->fd, GAME_END_COMMAND, strlen(GAME_END_COMMAND));
                sleep(1);
                scoreUpdate(currentPlayer->game, currentPlayer->fd);
                pthread_barrier_wait(&(currentPlayer->game->barrier));
                currentPlayer->game = NULL;
            } else {
                pthread_barrier_wait(&(currentPlayer->game->barrier));
                sleep(1);
                scoreUpdate(currentPlayer->game, currentPlayer->fd);
                pthread_barrier_wait(&(currentPlayer->game->barrier));
                sleep(1);

                int *shuffler = randomArray(NO_OF_ANSWERS);
                char newQuestionCommand[MAX_QUESTION_COMMAND_SIZE];
                strcpy(newQuestionCommand, GAME_NEWQUESTION_COMMAND);
                sprintf(newQuestionCommand, "%s|%s", newQuestionCommand,
                        currentPlayer->game->currentQuestion->question);
                for (int index = 0; index < NO_OF_ANSWERS; index++) {
                    if (shuffler[index] == 0)
                        correctAnswer = index;
                    sprintf(newQuestionCommand, "%s|%s", newQuestionCommand,
                            currentPlayer->game->currentQuestion->answers[shuffler[index]]);
                }

                free(shuffler);

                strcat(newQuestionCommand, "\n");
                write(currentPlayer->fd, newQuestionCommand,
                      strlen(newQuestionCommand));
                pthread_barrier_wait(&(currentPlayer->game->barrier));
                pthread_barrier_wait(&(currentPlayer->game->barrier));
            }
        }

        int readRet = read(currentPlayer->fd, buffer, READ_BUF_SIZE);

        char command[COMMAND_SIZE];
        char username[USER_SIZE];
        char password[PASSWORD_SIZE];

        if (readRet > 0) {
            buffer[readRet] = '\0';
            sscanf(buffer, "%s %s %s", command, username, password);
            printf("Received %s|%s|%s\n", command, username, password);
        } else
            command[0] = '\0';

        // Check if username and password exists in database if client selected
        // login command
        if (strcmp(command, LOGIN_COMMAND) == 0) {
            handle_login_command(db, currentPlayer, username, password);
        } else if (strcmp(command, NEWUSER_COMMAND) == 0) {
            handle_newuser_command(db, currentPlayer, username, password);
        } else if (strcmp(command, LOGOUT_COMMAND) == 0) {
            handle_logout_command(currentPlayer);
        } else if (strcmp(command, ANSWER_COMMAND) == 0) {
            handle_answer_command(currentPlayer, username, correctAnswer);
        } else if (strcmp(command, QUIT_COMMAND) == 0) {
            printf("[server] The client with" "%d descriptor has quit.\n", currentPlayer->fd);
            close(currentPlayer->fd); // Closing the connection
            pthread_mutex_lock(&mutex);
            int playerId = 0;
            for ( ; playerId < MAX_LOGGED_PLAYERS; playerId++)
                if (players[playerId] == currentPlayer)
                    break;
            players[playerId] = NULL;
            pthread_mutex_unlock(&mutex);
            deletePlayer(currentPlayer);
            break;
        } else {
            if (readRet > 0)
                printf("Unknown command\n");
        }
    }
    sqlite3_close(db);

    pthread_detach(pthread_self());
    return NULL;
}

int main () {

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Mutex init failed");
        exit(0);
    }

    int socket_fd;
    if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server] Socket() error.\n");
        return errno;
    }

    // Setting option SO_REUSEADDR for socket
    int optval=1; // Option used for setsockopt()
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Data structures for clients and server
    struct sockaddr_in server;
    bzero(&server, sizeof(server));

    // Take values for structure server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Attaching socket
    if (bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[server] bind() error.\n");
        return errno;
    }

    // The server is waiting for clients
    if (listen(socket_fd, 5) == -1) {
        perror("[server] listen() error.\n");
        return errno;
    }

    printf("[server] We are waiting to the port %d...\n", PORT);

    int clientId = 0;
    pthread_t thread_id[100];    //Id for thread that will be created

    // Concurrent server
    while (1) {
        printf("Beginning iteration...\n");
        fflush(stdout);
        // Prepare client structure
        struct sockaddr_in from;
        int len = sizeof(from);
        printf("Declared stuff\n");
        fflush(stdout);
        bzero(&from, sizeof(from));
        printf("Bzeroed\n");
        fflush(stdout);

        // If a client is coming, the connection is accepted
        printf("Accepting...");
        fflush(stdout);
        int client = accept(socket_fd, (struct sockaddr *)&from, (socklen_t *)&len);
        printf("Accepted\n");
        fflush(stdout);
        if (client < 0) {
            perror("[server] accept() error.\n");
            continue;
        }

        fcntl(client, F_SETFL, fcntl(client, F_GETFL, 0) | O_NONBLOCK);
        struct Player *player = newPlayer(client);
        printf("New player created\n");
        pthread_mutex_lock(&mutex);
        printf("Lock aquired. Adding new player...\n");

        int playerId = 0;
        for ( ; playerId < MAX_LOGGED_PLAYERS; playerId++)
            if (players[playerId] == NULL)
                break;

        printf("Iteration ended at %d\n", playerId);

        players[playerId] = player;

        printf("Player added\n");

        pthread_mutex_unlock(&mutex);

        printf("Lock released\n");

        struct Thread_data *thread_data = malloc(sizeof(struct Thread_data));
        thread_data->client_fd = client;

        pthread_create(&thread_id[clientId++], NULL, &treat, player);

        printf("Thread spawn\n");

        if (clientId == 99)
            exit(0);
        printf("[server] The client with descriptor %d has been connected\n", client);
    }
}
