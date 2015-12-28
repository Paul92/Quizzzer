#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    sqlite3 *db;
    char *zErrMsg = 0;
    char *sql;
    int rc;
    
    // Open database
    rc = sqlite3_open("QuizzGame.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stdout, "Database opened succesfully\n");
    }

    // Create Questions table
    sql = "CREATE TABLE Questions(" \
           "id INT PRIMARY KEY NOT NULL," \
           "question CHAR(100)," \
           "right_answer CHAR(30)," \
           "wrong_answer1 CHAR(30)," \
           "wrong_answer2 CHAR(30)," \
           "wrong_answer3 CHAR(30) );";

    // Execute first statement
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created succesfully\n");
    }
    sqlite3_close(db);
	
    int rc1;
    // Open database
    rc1 = sqlite3_open("QuizzGame.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stdout, "Database opened succesfully\n");
    }

    char *sql1;
    // Create Users table
    sql1 =  "CREATE TABLE Users(" \
            "user CHAR(20) PRIMARY KEY NOT NULL," \
            "password CHAR(20)," \
            "nr_of_games_played INT);";

    // Execute second statement
    rc1 = sqlite3_exec(db, sql1, callback, 0, &zErrMsg);
    if (rc1 != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stderr, "Table created succesfully\n");
    }
    sqlite3_close(db);
    return 0;
}
