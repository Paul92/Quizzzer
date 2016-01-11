#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <pthread.h>

#define PORT 9898

#define READ_BUF_SIZE 1024

#define PASSWORD_SIZE        128    // Maxim dimension of password
#define USER_SIZE            128    // Maxim dimensions of user
#define COMMAND_SIZE         128    // Size of command
#define SQL_COMMAND_MAX_SIZE 512    // Size of sql commands
#define MAX_LOGGED_PLAYERS   20     // The maxim number of players connected
#define NO_GAME_PLAYERS      2
#define NO_QUESTIONS         10
#define DATABASE_NAME        "QuizzGame.db"

#define LOGIN_COMMAND   "login"
#define NEWUSER_COMMAND "newuser"
#define LOGOUT_COMMAND  "logout"
#define QUIT_COMMAND    "quit"

struct Thread_data {
    int client_fd; // Descriptor returned by accept function
};
// Defining a structure which retains details about players
struct Player {
    int fd;
    char *username;
    int loggedIn;
};

// Defining a structure which retains details about game
struct Game {
    int playerIds[NO_GAME_PLAYERS];
    int answers[NO_GAME_PLAYERS];
    int questionCount;
    char *currentQuestion;
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

char *getNewQuestion(sqlite3 *db) {

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

void *game_master(void *arg) {
    struct Game *game = arg;

    sqlite3 *db = open_db();

    while (game->questionCount < NO_QUESTIONS) {
        game->currentQuestion = getNewQuestion(db);
    }


}

void *treat(void *arg) {

    struct Thread_data *data = arg;

    struct Player *currentPlayer = malloc(sizeof(currentPlayer));

    currentPlayer->fd       = data->client_fd;
    currentPlayer->username = NULL;
    currentPlayer->loggedIn = 0;

    pthread_mutex_lock(&mutex);

    int playerId = 0;
    for ( ; playerId < MAX_LOGGED_PLAYERS; playerId++)
        if (players[playerId] == NULL)
            break;

    players[playerId] = currentPlayer;

    pthread_mutex_unlock(&mutex);

    sqlite3 *db = open_db();

    char buffer[READ_BUF_SIZE];
    buffer[read(data->client_fd, buffer, READ_BUF_SIZE)] = '\0';

    char command[COMMAND_SIZE];
    char username[USER_SIZE];
    char password[PASSWORD_SIZE];
    sscanf(buffer, "%s %s %s", command, username, password);

    char *zErrMsg = NULL;
    char sql_command[SQL_COMMAND_MAX_SIZE];

    while (1) {
        // Check if username and password exists in database if client selected
        // login command
        if (strcmp(command, LOGIN_COMMAND) == 0) {
            sprintf(sql_command, "SELECT * FROM Users WHERE "
                            "user = \"%s\" AND password = \"%s\";",
                    username, password);
            strcpy(sql_command, "SELECT * FROM users;");
            printf("Doing login\n");
            int login_query_return = 0;
            if (sqlite3_exec(db, sql_command, login_query,
                             &login_query_return, &zErrMsg) != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                exit(0);
            }
            // If username and password is not good, then will print an error
            // mesage
            if (login_query_return == 0) {
                printf("Login failed.\n");
                write(data->client_fd, "FAILED\n", 8);
            } else {
                // Login succed
                printf("Login successful.\n");
                write(data->client_fd, "OK\n", 4);

                pthread_mutex_lock(&mutex);
                players[playerId]->loggedIn = 1;
                pthread_mutex_unlock(&mutex);
            }
        } else if (strcmp(command, NEWUSER_COMMAND) == 0) {
            // Inserting new user in database
            sprintf(sql_command, "INSERT INTO Users(user, password) "
                            "VALUES (\"%s\", \"%s\");",
                    username, password);
            // If insertion has failed, then an error message will be printed
            if (sqlite3_exec(db, sql_command, register_query, NULL,
                             &zErrMsg) != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                exit(0);
            } else {
                fprintf(stdout, "You are now registered!\n");
                write(data->client_fd, "OK\n", 4);
            }
        } else if (strcmp(command, LOGOUT_COMMAND) == 0) {
            if (players[playerId]->loggedIn) {
                printf("[server] The client with"
                               "%d descriptor has been disconected.\n", data->client_fd);
                close(data->client_fd); // Closing the connection
            } else {
                printf("[server] The client with %d descriptor"
                        "is not logged in\n", data->client_fd);
            }
            break;
        }
    }

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

    // Ataching socket
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
        // Prepare client structure
        struct sockaddr_in from;
        int len = sizeof(from);
        bzero(&from, sizeof(from));

        // If a client is coming, the connection is accepted
        int client = accept(socket_fd, (struct sockaddr *)&from, (socklen_t *)&len);
        if (client < 0) {
            perror("[server] accept() error.\n");
            continue;
        }

        struct Thread_data *thread_data = malloc(sizeof(struct Thread_data));
        thread_data->client_fd = client;

        pthread_create(&thread_id[clientId++], NULL, &treat, thread_data);

        if (clientId == 99)
            exit(0);
        printf("[server] The client with descriptor %d has been connected"
                "from address %s.\n",client, conv_addr(from));
        fflush(stdout);
    }
}
