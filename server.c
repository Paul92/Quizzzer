#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define PORT 9856

#define READ_BUF_SIZE 1024

#define PASSWORD_SIZE 128
#define USER_SIZE     128
#define COMMAND_SIZE  128

#define LOGIN_COMMAND "login"
#define NEWUSER_COMMAND "newuser"
#define LOGOUT_COMMAND "logout"

static int select_query(void *data,int argc, char **argv, char **azColName){
	if (argv==NULL)
		{return 0;}
	else
		{return 4;}
}

static int nothing_to_do_query(void *NotUsed, int argc, char **argv, char **azColName){
   return 0;
}

extern int errno;        // eroarea returnata de unele apeluri

// functie de convertire a adresei IP a clientului in sir de caractere 
char * conv_addr (struct sockaddr_in address) {
    static char str[25];
    char port[7];

    // adresa IP a clientului 
    strcpy (str, inet_ntoa (address.sin_addr));
    
    // portul utilizat de client
    bzero (port, 7);
    sprintf (port, ":%d", ntohs (address.sin_port));    
    strcat (str, port);
    return (str);
}

int main () {
    
    sqlite3 *db;

    // Open database
    if (sqlite3_open("QuizzGame.db", &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stdout, "Database opened succesfully\n");
    }

    /*struct player {
        int filedescriptor;
        char* username;
    };
    player v[100];*/


    struct sockaddr_in server;  // structurile pentru server si clienti
    struct sockaddr_in from;

    fd_set readfds;             // multimea descriptorilor de citire
    fd_set actfds;              // multimea descriptorilor activi
    struct timeval tv;          // structura de timp pentru select()
    int sd, client;             // descriptori de socket
    int optval=1;               // optiune folosita pentru setsockopt() 
    int fd;                     // descriptor folosit pentru 
                                // parcurgerea listelor de descriptori
    
    int nfds;                   // numarul maxim de descriptori
    int len;                    // lungimea structurii sockaddr_in

    // creare socket 
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("[server] Eroare la socket().\n");
        return errno;
    }

    // setam pentru socket optiunea SO_REUSEADDR
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // pregatim structurile de date
    bzero (&server, sizeof (server));

    // umplem structura folosita de server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // atasam socketul
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1) {
        perror ("[server] Eroare la bind().\n");
        return errno;
    }

    // punem serverul sa asculte daca vin clienti sa se conecteze
    if (listen (sd, 5) == -1) {
        perror ("[server] Eroare la listen().\n");
        return errno;
    }
  
    // completam multimea de descriptori de citire
    FD_ZERO (&actfds);          // initial, multimea este vida
    FD_SET (sd, &actfds);       // includem in multime socketul creat

    tv.tv_sec = 1;              // se va astepta un timp de 1 sec.
    tv.tv_usec = 0;
  
    // valoarea maxima a descriptorilor folositi
    nfds = sd;

    printf ("[server] Asteptam la portul %d...\n", PORT);
    fflush (stdout);
        
    // servim in mod concurent clientii...
    while (1) {
        // ajustam multimea descriptorilor activi (efectiv utilizati)
        bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

        // apelul select()
        if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0) {
            perror ("[server] Eroare la select().\n");
            return errno;
        }
        // vedem daca e pregatit socketul pentru a-i accepta pe clienti
        if (FD_ISSET (sd, &readfds)) {
            // pregatirea structurii client
            len = sizeof (from);
            bzero (&from, sizeof (from));

            // a venit un client, acceptam conexiunea
            client = accept (sd, (struct sockaddr *) &from, &len);

            // eroare la acceptarea conexiunii de la un client
            if (client < 0) {
                perror ("[server] Eroare la accept().\n");
                continue;
            }

            if (nfds < client) // ajusteaza valoarea maximului
              nfds = client;
                
            // includem in lista de descriptori activi si acest socket
            FD_SET (client, &actfds);

            printf("[server] S-a conectat clientul cu descriptorul %d,"
                    "de la adresa %s.\n",client, conv_addr (from));
            fflush (stdout);
        }
        // vedem daca e pregatit vreun socket client pentru a trimite raspunsul
        for (fd = 0; fd <= nfds; fd++) { // parcurgem multimea de descriptori
            // este un socket de citire pregatit?
            if (fd != sd && FD_ISSET (fd, &readfds)) {
                char buffer[READ_BUF_SIZE];
                read(fd, buffer, READ_BUF_SIZE);
                char command[COMMAND_SIZE];
                char username[USER_SIZE];
                char password[PASSWORD_SIZE];
                char * sql;
                char *zErrMsg;
                sscanf(buffer, "%s %s %s", command, username, password);
                if (strcmp(command, LOGIN_COMMAND)) {
                    sql = sprintf(db, "SELECT * FROM Users WHERE username = %s"
                                  "AND password = %S;", username, password);
                    if (sqlite3_exec(db, sql, select_query, NULL, &zErrMsg) != SQLITE_OK ) {
                        fprintf(stderr, "SQL error: %s\n",zErrMsg);
                        sqlite3_free(zErrMsg);
                    }
                } else if (strcmp(command, NEWUSER_COMMAND)) {
                    sql = sprintf(db, "INSERT INTO Users(user, password,"
                                  "nr_of_games_played) VALUES (%s, %s);",
                                  username, password);
                    if (sqlite3_exec(db, sql, nothing_to_do_query, 0, &zErrMsg) != SQLITE_OK ) {
                        fprintf(stderr, "SQL error: %s\n",zErrMsg);
                        sqlite3_free(zErrMsg);
                    } else {
                        fprintf(stdout, "You are now registered!\n");
                    }
                } else if (strcmp(command, LOGOUT_COMMAND)) {
                    printf ("[server] S-a deconectat clientul cu "
                            "descriptorul %d.\n",fd);
                    fflush (stdout);
                    close (fd);           // inchidem conexiunea cu clientul
                    FD_CLR (fd, &actfds); // scoatem si din multime
                }

            }
        }
    }
}