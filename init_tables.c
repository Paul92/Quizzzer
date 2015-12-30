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
                         "user CHAR(30) PRIMARY KEY NOT NULL," \
                         "password CHAR(30) NOT NULL,"         \
                         "nr_of_games_played INT DEFAULT 0 NOT NULL);";

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

    //Insert rows in Questions table
    char *insert_questions = "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (1, 'Care este expresia generala pentru " \
                             "toate sistemele de retea si de transfer fara " \
                             "cablu?', 'a. Wireless', 'b. Infrarosu', 'c. Unde'," \
                             "'d. Bluetooth');" \

                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (2, 'Cum se numeste un mic fisier cu " \
                             "informatii de tip text?', 'a. Cookie', " \
                             "'b. Tools', 'c. History', 'd. Tab');" \

                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (3, 'Cum se numeste o pagina de " \
                             "internet?', 'a. Website', 'b. HTML', 'c. Web'," \
                             "'d. Site');" \

                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (4, 'Care este prescurtarea denumirii " \
                             "vitezei de transfer pe biti pe secunda?', " \
                             "'a. bps', 'b. tps', 'c. dps', 'd. mps');" \
                             
                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (5, 'Cum se numeste cel mai raspandit " \
                             "limbaj de programare?', 'a. BASIC', 'b. C', " \
                             "'c. C++', 'd. Pascal');" \

                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (6, 'Cum se numeste o pagina de " \
                             "internet?', 'a. Website', 'b. HTML', 'c. Web', " \
                             "'d. Site');" \
                             
                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (7, 'Ce pasare poate imita vocile altor " \
                             "pasari din padure?', 'a. Gaita', " \
                             "'b. Papagalul', 'c. Vrabia', 'd. Mierla');" \

                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (8, 'Cum se numeste pigmentul verde din " \
                             "plante?', 'a. Clorofila', 'b. Petala', " \
                             "'c. Frunza', 'd. Polen');" \

                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (9, 'Ce copac este considerat sacru in " \
                             "China?', 'a. Gingko', 'b. Bambusul', " \
                             "'c. Pinul', 'd. Gorunul');" \

                             "INSERT INTO Questions (id, question, " \
                             "right_answer, wrong_answer1, wrong_answer2, " \
                             "wrong_answer3) " \
                             "VALUES (10, 'Unde se afla Waterloo?', " \
                             "'a. Belgia', 'b. Franta', 'c. Anglia', " \
                             "'d.Austria');";                         
    
    // Execute third statement
    if (sqlite3_exec(db, insert_questions, callback, 0, &zErrMsg) != SQLITE_OK) {
        fprintf(stderr, "Could not insert rows in Questions table.\nSQL error: %s\n",
                         zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    } else {
        fprintf(stderr, "Rows inserted succesfully\n");
    }
    //Insert rows in Users table
    char* insert_users = "INSERT INTO Users (user, password, " \
                         "nr_of_games_played) " \
                         "VALUES (silvia95, anamaria96, 2);" \

                         "INSERT INTO Users (user, password, " \
                         "nr_of_games_played) " \
                         "VALUES (gadyPhol, apollo18, 1);" \

                         "INSERT INTO Users (user, password, " \
                         "nr_of_games_played) " \
                         "VALUES (silvia, 123456, 2);" ;

    // Execute last statement
    if (sqlite3_exec(db, insert_users, callback, 0, &zErrMsg) != SQLITE_OK) {
        fprintf(stderr, "Couldn't insert rows in Users table.\nSQL error: %s\n",
                zErrMsg);
        sqlite3_free(zErrMsg);
        exit(0);
    } else {
        fprintf(stderr, "Rows inserted succesfully\n");
    }
                         
    sqlite3_close(db);
    return 0;
}
