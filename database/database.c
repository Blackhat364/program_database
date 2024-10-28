#include "database_library.h"
#include "errors_file.h"
#include "hash_library.h"

#define TEST 
#define RUN2



Tables database;
Tables* database_ptr = &database;
Tables database2;

int main() {
    #ifdef TEST
    handle_error(init_database(database_ptr),"init_database", " malloc memory error");
    clean_file("error_log.txt");
    Users user = {
        .user_id = DEFAULT_INT,
        .user_computer = 100,
        .user_email = "dror3646@gmail",
        .user_name = "dror",
        .user_phon = "0507510579",
        .user_permission = 1,
        .user_pass = "d1r2o3r4",
    };
    add_row(database_ptr, USERS_TABLE, LEVEL1, user);
    for (int i = 0;i <10;i++) {
        add_row(database_ptr, USERS_TABLE,LEVEL0);
    }
    int users_id[102];
    add_attribute(database_ptr,USERS_TABLE,1,USER_NAME,"dor");
    add_attribute(database_ptr, USERS_TABLE, 1, USER_PASS, "1234");
    add_attribute(database_ptr, USERS_TABLE, 3, USER_PASS, "1234");
    handle_error(search_by_attribute(database, USERS_TABLE, USER_NAME, "dodr", users_id),"search_by_attribure", "not found attribut");

    int i = 100;
    add_attribute(database_ptr, USERS_TABLE, 4, USER_COMPUTER,&i );
    add_attribute(database_ptr, USERS_TABLE, 3, USER_COMPUTER, &i );
    print_database(database, ALLTABLES);
    print_by_id(database, USERS_TABLE, 10);

    pot_in_hash_table(database_ptr, HASH_TABLE_NAME, database.users[0]);
    pot_in_hash_table(database_ptr, HASH_TABLE_NAME, database.users[1]);
    pot_in_hash_table(database_ptr, HASH_TABLE_PASS, database.users[0]);
    print_hash_tabel(database_ptr, HASH_TABLE_NAME);
    print_hash_tabel(database_ptr, HASH_TABLE_PASS);
   
    printf("user_id in : %c\n", database.database_settings.user_id_list[11]);

    printf("user_pass :%s user_id :%d\n", database.users[0].user_pass, search_in_hash_table(database_ptr, HASH_TABLE_PASS,database.users[0].user_pass));
    
    printf("matches range search:%d\n",range_search_databse(database, 0,3));

    
    int matches = search_by_attribute(database, USERS_TABLE, USER_COMPUTER, 100, users_id);
    printf("matches = %d\n", matches);
    printf("id : %d\n",users_id[matches-1]);
    search_by_attribute(database, USERS_TABLE, USER_NAME, "dor", users_id);
    strcpy(database.database_settings.database_save_file_path, "C:\\Users\\drorPc\\Desktop\\Memory management projec\\database_Memory_management_projec\\database\\save_database_file.b");
    
    add_attribute(database_ptr, USERS_TABLE, 10, USER_NAME, "youve");
    save_database_sd(database);
    add_row(database_ptr, USERS_TABLE, LEVEL0);
    add_attribute(database_ptr, USERS_TABLE, 1, USER_NAME, "youvde");
    print_database(database, USERS_TABLE);
    if (load_from_sd(&database) == E_NOT_E) {
        // Proceed to print the database
        print_database(database, USERS_TABLE);
        print_hash_tabel(&database, HASH_TABLE_NAME);
    }
    free_database(database_ptr);

    
    #endif // TEST
    #ifdef RUN2
    init_database(&database2);
    strcpy(database2.database_settings.save_database_file_path, "C:\\Users\\drorPc\\Desktop\\Memory management projec\\database_Memory_management_projec\\database\\save_database_file.b");
    if (load_from_sd(&database2) == E_NOT_E) {
        // Proceed to print the database
        print_database(database2, USERS_TABLE);
    }
    printf(" user_pass: %s user_id: %d", database2.users[0].user_pass,search_in_hash_table(&database2, HASH_TABLE_PASS, database2.users[0].user_pass));
    matches = search_by_attribute(database, USERS_TABLE, USER_COMPUTER, 100, users_id);
    //Error_struct error;
    //get_last_error("error_log.txt", &error);
    #endif 
    return 0;
}


//printf("%s\n", (attribute_type(DATA_PATH) == 'i') ? "the function return int":"function return string");

