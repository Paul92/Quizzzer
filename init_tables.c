#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++)
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    sqlite3 *db;

    // Open database
    if (sqlite3_open("QuizzGame.db", &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stdout, "Database opened succesfully\n");
    }

    // Create Questions table
    char *create_questions = "CREATE TABLE Questions("      \
                             "id INT PRIMARY KEY NOT NULL," \
                             "question      CHAR(300),"     \
                             "right_answer  CHAR(300),"     \
                             "wrong_answer1 CHAR(300),"     \
                             "wrong_answer2 CHAR(300),"     \
                             "wrong_answer3 CHAR(300) );";

    char *create_users = "CREATE TABLE Users("                 \
                         "user CHAR(20) PRIMARY KEY NOT NULL," \
                         "password CHAR(20),"                  \
                         "nr_of_games_played INT);";

    char *zErrMsg = NULL;

    // Execute create questions
    if (sqlite3_exec(db, create_questions, callback, 0, &zErrMsg) != SQLITE_OK) {
        fprintf(stderr, "Could not create table Questions.\nSQL error: %s\n",
                         zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    } else {
        fprintf(stdout, "Table Questions created succesfully\n");
    }

    // Execute second statement
    if (sqlite3_exec(db, create_users, callback, 0, &zErrMsg) != SQLITE_OK) {
        fprintf(stderr, "Could not create table Users.\nSQL error: %s\n",
                         zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    } else {
        fprintf(stderr, "Table Users created succesfully\n");
    }

    sqlite3_close(db);
    return 0;
}
