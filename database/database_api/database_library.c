/*
* dror cohen 11/09/2024
*/
#include "database_library.h"
#include "errors_file.h"
#include "hash_library.h"

int num_of_users = 0;
int num_of_users_data = 0;

int init_database(Tables* database) {//init database
    database->users = (Users*)malloc(DEFAULT_SIZE * sizeof(Users));
    database->users_data = (Users_data*)malloc(DEFAULT_SIZE * sizeof(Users_data));
    database->database_settings.user_data_id_list = (char*)malloc(DEFAULT_SIZE * sizeof(char));
    database->database_settings.user_id_list = (char*)malloc(DEFAULT_SIZE * sizeof(char));
    database->database_settings.users_table_size = DEFAULT_SIZE;

    if (database->users == NULL) {
        printf("not can malloc memory error\n");
        free_database(database);
        exit(E_MAM_E);
    }
    if (database->users_data == NULL) {
        printf("not can malloc memory error\n");
        free_database(database);
        exit(E_MAM_E);
    }if (database->database_settings.user_id_list == NULL) {
        printf("not can malloc memory error\n");
        free_database(database);
        exit(E_MAM_E);
    }if (database->database_settings.user_data_id_list == NULL) {
        printf("not can malloc memory error\n");
        free_database(database);
        exit(E_MAM_E);
    }

    for (int i = 0;i < DEFAULT_SIZE;i++) {
        database->database_settings.user_id_list[i] = '0';
    }
    for (int i = 0;i < DEFAULT_SIZE;i++) {
        database->database_settings.user_data_id_list[i] = '0';
    }
    database->database_settings.users_table_size = DEFAULT_SIZE;
    database->database_settings.users_data_table_size = DEFAULT_SIZE;
    database->database_settings.num_of_managers = NUM_OF_MANAGERS;
    database->user_data_primary_keys = 0;
    database->user_data_uinque_keys = 0;
    database->user_primary_keys = 0;
    database->user_uinque_keys = 0;
    database->database_settings.num_of_users = 0;
    database->database_settings.num_of_users_data = 0;
    init_managers(database);
    database->managers_table[0].manager_id = 0;
    database->managers_table[0].manager_permission = ADMIN_PERMISSION;
    strcpy(database->managers_table[0].manager_name,"dror_admin");
    strcpy(database->managers_table[0].manager_pass,"d1r2o3r4");
    strcpy(database->managers_table[0].manager_email,"dror3646@gmail.com");
    strcpy(database->managers_table[0].manager_phon,"0532455705");
    strcpy(database->database_settings.database_save_file_path,"D:\\database\\database_save_file.b");
    strcpy(database->database_settings.log_erros_files_path, "D:\\database\\errror_log_file.txt");
    strcpy(database->database_settings.database_settings_file_path, "database_settings_file.txt");
    set_uinque_keys(database, USERS_TABLE, USER_PASS);
    set_uinque_keys(database, USERS_TABLE, USER_NAME);
    set_uinque_keys(database, USERS_TABLE, USER_PHON);
    set_uinque_keys(database, USERS_TABLE, USER_EMAIL);
    init_hash_table(database);
    return E_NOT_E;
}
int save_database_sd(Tables* database) {
    FILE* database_file;
    char file_path[200];
    strcpy(file_path, database->database_settings.database_save_file_path); // Use the file path from Database_settings

    // Open the database file in binary write mode
    database_file = fopen(file_path, "wb");
    if (database_file == NULL) {
        printf("Error opening file for saving: %s\n", file_path);
        return E_OP_FILE;
    }

    // Step 1: Save fixed-size fields of Database_settings
    if (fwrite(&(database->database_settings.log_erros_files_path), sizeof(char), 200, database_file) != 200 ||
        fwrite(&(database->database_settings.database_save_file_path), sizeof(char), 200, database_file) != 200 ||
        fwrite(&(database->database_settings.database_settings_file_path), sizeof(char), 200, database_file) != 200 ||
        fwrite(&(database->database_settings.users_table_size), sizeof(int), 1, database_file) != 1 ||
        fwrite(&(database->database_settings.users_data_table_size), sizeof(int), 1, database_file) != 1 ||
        fwrite(&(database->database_settings.num_of_users), sizeof(int), 1, database_file) != 1 ||
        fwrite(&(database->database_settings.num_of_users_data), sizeof(int), 1, database_file) != 1 ||
        fwrite(&(database->database_settings.num_of_managers), sizeof(int), 1, database_file) != 1) {
        printf("Error writing database settings to file\n");
        fclose(database_file);
        return E_WRITE_FILE;
    }

    // Step 2: Save the user_id_list (dynamic array)
    if (database->database_settings.user_id_list != NULL) {
        if (fwrite(database->database_settings.user_id_list, sizeof(char), database->database_settings.num_of_users, database_file) != database->database_settings.num_of_users) {
            printf("Error writing user ID list to file\n");
            fclose(database_file);
            return E_WRITE_FILE;
        }
    }

    // Step 3: Save the user_data_id_list (dynamic array)
    if (database->database_settings.user_data_id_list != NULL) {
        if (fwrite(database->database_settings.user_data_id_list, sizeof(char), database->database_settings.num_of_users_data, database_file) != database->database_settings.num_of_users_data) {
            printf("Error writing user data ID list to file\n");
            fclose(database_file);
            return E_WRITE_FILE;
        }
    }

    // Step 4: Save the users table
    if (fwrite(database->users, sizeof(Users), database->database_settings.users_table_size, database_file) != database->database_settings.users_table_size) {
        printf("Error writing users to file\n");
        fclose(database_file);
        return E_WRITE_FILE;
    }

    // Step 5: Save the users_data table
    if (fwrite(database->users_data, sizeof(Users_data), database->database_settings.users_data_table_size, database_file) != database->database_settings.users_data_table_size) {
        printf("Error writing users data to file\n");
        fclose(database_file);
        return E_WRITE_FILE;
    }

    // Step 6: Save the hash tables
    for (int i = 0; i < database->hash_table_setting.number_of_table; i++) {
        if (fwrite(database->hash_tables[i], sizeof(int), DEFAULT_SIZE, database_file) != DEFAULT_SIZE) {
            printf("Error writing hash table %d to file\n", i);
            fclose(database_file);
            return E_WRITE_FILE;
        }
    }

    fclose(database_file);
    printf("Database saved successfully.\n");
    return E_NOT_E;
}





int load_from_sd(Tables* database) {
    init_database(database);
    FILE* database_file;
    char file_path[200];
    strcpy(file_path, database->database_settings.database_save_file_path); // Use the file path from Database_settings

    // Open the database file in binary read mode
    database_file = fopen(file_path, "rb");
    if (database_file == NULL) {
        printf("Error opening file for loading: %s\n", file_path);
        return E_OP_FILE;
    }

    // Step 1: Load fixed-size fields of Database_settings
    if (fread(&(database->database_settings.log_erros_files_path), sizeof(char), 200, database_file) != 200 ||
        fread(&(database->database_settings.database_save_file_path), sizeof(char), 200, database_file) != 200 ||
        fread(&(database->database_settings.database_settings_file_path), sizeof(char), 200, database_file) != 200 ||
        fread(&(database->database_settings.users_table_size), sizeof(int), 1, database_file) != 1 ||
        fread(&(database->database_settings.users_data_table_size), sizeof(int), 1, database_file) != 1 ||
        fread(&(database->database_settings.num_of_users), sizeof(int), 1, database_file) != 1 ||
        fread(&(database->database_settings.num_of_users_data), sizeof(int), 1, database_file) != 1 ||
        fread(&(database->database_settings.num_of_managers), sizeof(int), 1, database_file) != 1) {
        printf("Error reading database settings from file\n");
        fclose(database_file);
        return E_READ_FILE;
    }

    // Now manually load the dynamic arrays

    // Load the user_id_list (dynamic array)
    if (database->database_settings.num_of_users > 0) {
        if (fread(database->database_settings.user_id_list, sizeof(char), database->database_settings.num_of_users, database_file) != database->database_settings.num_of_users) {
            printf("Error reading user ID list from file\n");
            fclose(database_file);
            return E_READ_FILE;
        }
    }

    // Load the user_data_id_list (dynamic array)
    if (database->database_settings.num_of_users_data > 0) {
        if (fread(database->database_settings.user_data_id_list, sizeof(char), database->database_settings.num_of_users_data, database_file) != database->database_settings.num_of_users_data) {
            printf("Error reading user data ID list from file\n");
            fclose(database_file);
            return E_READ_FILE;
        }
    }

    // Step 2: Load the users table
    if (fread(database->users, sizeof(Users), database->database_settings.users_table_size, database_file) != database->database_settings.users_table_size) {
        printf("Error reading users from file\n");
        fclose(database_file);
        return E_READ_FILE;
    }

    // Step 3: Load the users_data table

    if (fread(database->users_data, sizeof(Users_data), database->database_settings.users_data_table_size, database_file) != database->database_settings.users_data_table_size) {
        printf("Error reading users data from file\n");
        fclose(database_file);
        return E_READ_FILE;
    }

    // Step 4: Load the hash tables

    for (int i = 0; i < database->hash_table_setting.number_of_table; i++) {
        if (fread(database->hash_tables[i], sizeof(int), DEFAULT_SIZE, database_file) != DEFAULT_SIZE) {
            printf("Error reading hash table %d from file\n", i);
            fclose(database_file);
            return E_READ_FILE;
        }
    }

    fclose(database_file);
    printf("Database loaded successfully.\n");
    return E_NOT_E;
}

int load_config_file(Tables* database,char *config_file_path) {
    if (config_file_path == NULL) {
        strcpy(database->database_settings.database_save_file_path, "C:\\database_save_file.b");
        strcpy(database->database_settings.log_erros_files_path, "C:\\errror_log_file.txt");
        strcpy(database->database_settings.database_settings_file_path, "C:\\database_settings_file.txt");

    }
}



void free_database(Tables *database){    
    free(database->users);
    free(database->users_data);
    free(database->database_settings.user_data_id_list);
    free(database->database_settings.user_id_list);
    free_hash_table(database);
}


int add_row(Tables* database, Flag table, Flag flag, ...) {
    if (table == USERS_TABLE) {
        if (database->database_settings.num_of_users == database->database_settings.users_table_size) {
            realloc_database(database, USERS_TABLE, database->database_settings.users_table_size + 10);
        }
        Users user;
        if ((flag & LEVEL1) == LEVEL1) {
            va_list args;
            va_start(args, flag);
            user = va_arg(args, Users);
            va_end(args);
        }
        else {
            // Initialize default values
            user.user_id = DEFAULT_INT;
            user.user_computer = DEFAULT_INT;
            strcpy(user.user_email, DEFAULT_STRING);
            strcpy(user.user_name, DEFAULT_STRING);
            strcpy(user.user_phon, DEFAULT_STRING);
            user.user_permission = DEFAULT_INT;
            strcpy(user.user_pass, DEFAULT_STRING);
        }

        // Get primary ID
        user.user_id = get_primary_id((*database), USERS_TABLE);
        if (user.user_id == E_NOT_F) {
            return E_NOT_F;

        }

        // Add the user to the database

        database->users[user.user_id] = user;
        database->database_settings.num_of_users++;
        

        return user.user_id;
    }
    else if (table == USERS_DATA_TABLE) {
        if (database->database_settings.num_of_users_data >= database->database_settings.users_data_table_size)
        {
            realloc_database(database, table, 10);
        }
        Users_data user_data;
        if ((flag & LEVEL1) == LEVEL1)
        {
            va_list args;
            va_start(args, flag);
            user_data = va_arg(args, Users_data);
            va_end(args);

        }
        else {
            user_data.data_id = DEFAULT_INT;
            user_data.user_id = DEFAULT_INT;
            user_data.data_size = DEFAULT_INT;
            user_data.computer_id = DEFAULT_INT;
            user_data.data_type = DEFAULT_INT;
            strcpy(user_data.data_path, DEFAULT_STRING);
        }
        user_data.data_id = get_primary_id((*database), table);
        if (user_data.data_id == E_N_EN_TA) {
            return E_N_EN_TA;
        }
        else {
            database->database_settings.num_of_users_data++;
            database->users_data[user_data.data_id] = user_data;
        }
        return user_data.data_id;

    }
    else {
        return E_N_EN_TA;
    }
    return E_NOT_E;
}
int add_attribute(Tables* database, Flag table, int id, Flag attribute, void* data) {
    if (!database || !data || id < 0) {
        return E_EN_IV_IN;  // Return error if invalid input
    }
    
    if (attribute == USER_ID) {
        printf("not can change a user_id!!!\n");
        return E_EN_IV_IN;
    }
    if (table == USERS_TABLE) {
        if(database->users[id].user_id == DEFAULT_INT ){
            printf("not found user by id :%d\n", id);
            return E_NOT_F;
        }

        if (check_uinque((*database), table, attribute) != E_NOT_E_U) {
            if (check_uniqueness(database, attribute, (char*)data) < 0) {
                printf("the attrubute that you want to add is not uinque!!!\n");
                return E_EN_IV_IN;
            }
        }

        switch (attribute) {
        case USER_NAME:
            strncpy(database->users[id].user_name, (char*)data, 255);
            database->users[id].user_name[254] = '\0';
            if (check_uinque((*database), table, attribute) != E_NOT_E_U) {
                pot_in_hash_table(database, HASH_TABLE_NAME, database->users[id]);
            }
            break;
        case USER_PASS:
            strncpy(database->users[id].user_pass, (char*)data, 255);
            database->users[id].user_pass[254] = '\0';
            if (check_uinque((*database), table, attribute) != E_NOT_E_U) {
                pot_in_hash_table(database, HASH_TABLE_PASS, database->users[id]);
            }
            break;
        case USER_EMAIL:
            strncpy(database->users[id].user_email, (char*)data, 255);
            database->users[id].user_email[254] = '\0';
            if (check_uinque((*database), table, attribute) != E_NOT_E_U) {
                pot_in_hash_table(database, HASH_TABLE_EMAIL, database->users[id]);
            }
            break;
        case USER_PHON:
            strncpy(database->users[id].user_phon, (char*)data, 11);
            database->users[id].user_phon[10] = '\0';
            if (check_uinque((*database), table, attribute) != E_NOT_E_U) {
                pot_in_hash_table(database, HASH_TABLE_PHON, database->users[id]);
            }
            break;
        case USER_PERMISSION:
            database->users[id].user_permission = *((int*)data);
            break;
        case USER_COMPUTER:
            database->users[id].user_computer = *((int*)data);
            break;
        default:
            printf("enter invalid attribute!!\n");
            return E_INVALID_ATTRIBUTE;
            break;

        }

    }
    else if (table == USERS_DATA_TABLE) {
        if (database->users_data[id].data_id == DEFAULT_INT) {
            printf("not found users_data by id :%d\n", id);
            return E_NOT_F;
        }
        switch (attribute) {
        case DATA_PATH:
            strncpy(database->users_data[id].data_path, (char*)data, 100);
            database->users_data[id].data_path[99] = '\0';
            break;
        case COMPUTER_ID:
            database->users_data[id].computer_id = *((int*)data);
            break;
        case DATA_SIZE:
            database->users_data[id].data_size = *((int*)data);
            break;
        case DATA_TYPE:
            database->users_data[id].data_type = *((int*)data);
            break;

        default:
            printf("enter invalid attribute!!\n");
            return E_INVALID_ATTRIBUTE;
            break;

        }
    }
    else {
        return E_N_EN_TA;
    }
    return E_NOT_E;
}
void print_line(int count) {
    for (int i = 0; i < count;i++) {
        printf("-");
    }
    printf("\n");

}
int print_database(Tables database,Flag table) {
    if (table == USERS_TABLE || table == ALLTABLES) {
        printf("\t\t\t\t\t\t users table!!\n\n");
        char my_string[400];
        for (int i = 0;i < database.database_settings.num_of_users;i++) {+

            sprintf(my_string, "|user_id:%d |user_name: %s |user_pass: %s |user_email: %s |user_phon: %s |user_permission: %d |user_coputer: %d |\n",database.users[i].user_id, database.users[i].user_name, database.users[i].user_pass, database.users[i].user_email, database.users[i].user_phon, database.users[i].user_permission, database.users[i].user_computer);
            size_t length = strlen(my_string);
            print_line(length-1);
            printf("%s", my_string);
            print_line(length-1);

        }



    }
    printf("\n\n");
    if (table == USERS_DATA_TABLE || table == ALLTABLES) {
        char my_string[400];
        printf("\t\t\t\t\t\t users_data table!!\n\n");
        for (int i = 0;i < database.database_settings.num_of_users_data;i++) {
            sprintf(my_string,"|data_id:%d |user_id: %d |data_type: %d |data_size: %d |user_path: %s |coputer_id: %d |\n", database.users_data[i].data_id, database.users_data[i].user_id, database.users_data[i].data_type, database.users_data[i].data_size, database.users_data[i].data_path, database.users_data[i].computer_id);
            size_t length = strlen(my_string);
            print_line(length-1);
            printf("%s",my_string);
            print_line(length-1);
        }
    }
    printf("\n\n");

    if (table != USERS_TABLE && table != USERS_DATA_TABLE && table != ALLTABLES){
       return E_N_EN_TA;
    }
        
    return E_NOT_E;   
}
int print_by_id(Tables database, Flag table, int id){
    if (table == USERS_TABLE) {
		if (database.users[database.database_settings.num_of_users -1].user_id < id) {
            printf("num of users:%d\n", database.database_settings.num_of_users);
			printf("not have a user by the id :%d\n", id);
			return E_NOT_F;
		}
        printf("\t\t\t\t table:%d user_id:%d\n",USERS_TABLE,id);
        char my_string[300];
        sprintf(my_string, "|user_id:%d |user_name: %s |user_pass: %s |user_email: %s |user_phon: %s |user_permission: %d |user_coputer: %d |\n", database.users[id].user_id, database.users[id].user_name, database.users[id].user_pass, database.users[id].user_email, database.users[id].user_phon, database.users[id].user_permission, database.users[id].user_computer);
        size_t length = strlen(my_string);
        print_line(length-1);
        printf("%s", my_string);
        print_line(length-1);
    }

    printf("\n\n");
    if (table == USERS_DATA_TABLE ) {
		if (database.users_data[database.database_settings.num_of_users_data].data_id < id) {
			printf("not have a user_data by the id :%d\n", id);
			return E_NOT_F;
		}
        char my_string[300];
        printf("\t\t\t\t table:%d user_data_id:%d\n", USERS_DATA_TABLE, id);
        sprintf(my_string, "|data_id:%d |user_id: %d |data_type: %d |data_size: %d |user_path: %s |coputer_id: %d |\n", database.users_data[id].data_id, database.users_data[id].user_id, database.users_data[id].data_type, database.users_data[id].data_size, database.users_data[id].data_path, database.users_data[id].computer_id);
        size_t length = strlen(my_string);
        print_line(length-1);
        printf("%s", my_string);
        print_line(length-1);

    }
    if (table != USERS_TABLE && table == USERS_DATA_TABLE && table == ALLTABLES) {
        return E_N_EN_TA;
    }
    
    return E_NOT_E;
}
char attribute_type(Flag attribute) {
    switch (attribute) {
    case USER_NAME:
        return 's';
        break;
    case USER_PASS:
        return 's';
        break;
    case USER_EMAIL:
        return 's';
        break;
    case USER_PHON:
        return 's';
        break;
    case USER_PERMISSION:
        return 'i';
        break;
    case USER_COMPUTER:
        return 'i';
        break;
    case USER_ID:
        return 'i';
        break;
    }
    switch (attribute) {
    case DATA_PATH:
        return 's';
        break;
    case COMPUTER_ID:
        return 'i';
        break;
    case DATA_SIZE:
        return 'i';
        break;
    case DATA_TYPE:
        return 'i';
        break;

    default:
        break;

    }
    return '3';
}
int set_primary_keys(Tables* database, Flag table, Flag attribute) {
    if (table == USERS_TABLE) {
        int primary_key = database->user_primary_keys;
        database->user_primary_keys = primary_key | attribute;
    }
    else if (table == USERS_DATA_TABLE)
    {
        int primary_key = database->user_data_primary_keys;
        database->user_data_primary_keys = primary_key | attribute;
    }
    return E_NOT_E;
}
int set_uinque_keys(Tables* database, Flag table, Flag attribute) {
    if (table == USERS_TABLE) {
        int uinque_key = database->user_uinque_keys;
        database->user_uinque_keys = uinque_key | attribute;
    }
    else if (table == USERS_DATA_TABLE)
    {
        int uinque_key = database->user_data_uinque_keys;
        database->user_data_uinque_keys = uinque_key | attribute;
    }
    else
    {
        return E_N_EN_TA;
    }
    return E_NOT_E;
}
int print_primary_keys(Tables database, Flag table) {
    //table 0
    if (table == USERS_TABLE || table == ALLTABLES) {
        int user_primary_keys = database.user_primary_keys;
        printf("----- user primary_keys ---------\n\n");
        if (user_primary_keys == 0) {
            printf("not have any primary keys!!\n");
        }
        if ((user_primary_keys & USER_ID) == USER_ID) {
            printf("user_id\n");
        }
        if ((user_primary_keys & USER_NAME) == USER_NAME) {
            printf("user_name\n");
        }
        if ((user_primary_keys & USER_PASS) == USER_PASS) {
            printf("user_pass\n");
        }
        if ((user_primary_keys & USER_EMAIL) == USER_EMAIL) {
            printf("user_email\n");
        }
        if ((user_primary_keys & USER_COMPUTER) == USER_COMPUTER) {
            printf("user_computer\n");
        }
        if ((user_primary_keys & USER_PHON) == USER_PHON) {
            printf("user_phon\n");
        }
        if ((user_primary_keys & USER_PERMISSION) == USER_PERMISSION) {
            printf("user_permission\n");
        }
    }
    printf("\n--------------------------------- \n\n\n");
    if (table == USERS_DATA_TABLE || table == ALLTABLES) {
        //table 1
        int user_primary_keys = database.user_data_primary_keys;
        printf("----- user_data primary_keys ---------\n\n");
        if (user_primary_keys == 0) {
            printf("not have any primary keys!!\n");
        }
        if ((user_primary_keys & DATA_ID) == DATA_ID) {
            printf("data_id\n");
        }
        if ((user_primary_keys & DATA_PATH) == DATA_PATH) {
            printf("data_path\n");
        }
        if ((user_primary_keys & DATA_SIZE) == DATA_SIZE) {
            printf("data_size!\n");
        }
        if ((user_primary_keys & DATA_TYPE) == DATA_TYPE) {
            printf("data_type\n");
        }
        if ((user_primary_keys & COMPUTER_ID) == COMPUTER_ID) {
            printf("data_computer_id\n");
        }
        printf("\n--------------------------------------\n\n");
    }

    

    return E_NOT_E;

}
int print_uinque_keys(Tables database,Flag table) {

    //table 0
    if (table == USERS_TABLE || table == ALLTABLES) {
        int user_uinque_keys = database.user_uinque_keys;
        printf("----- user uinque keys ---------\n\n");
        if (user_uinque_keys == 0) {
            printf("not have any uinque keys!!\n");
        }
        if ((user_uinque_keys & USER_ID) == USER_ID) {
            printf("user_id\n");
        }
        if ((user_uinque_keys & USER_NAME) == USER_NAME) {
            printf("user_name\n");
        }
        if ((user_uinque_keys & USER_PASS) == USER_PASS) {
            printf("user_pass\n");
        }
        if ((user_uinque_keys & USER_EMAIL) == USER_EMAIL) {
            printf("user_email\n");
        }
        if ((user_uinque_keys & USER_COMPUTER) == USER_COMPUTER) {
            printf("user_computer\n");
        }
        if ((user_uinque_keys & USER_PHON) == USER_PHON) {
            printf("user_phon\n");
        }
        if ((user_uinque_keys & USER_PERMISSION) == USER_PERMISSION) {
            printf("user_permission\n");
        }
        printf("\n--------------------------------- \n\n\n");
        
    }
    //table 1
    if (table == USERS_DATA_TABLE || table == ALLTABLES) {
        int user_uinque_keys = database.user_data_uinque_keys;
        printf("----- user_data uinque keys ---------\n\n");
        if (user_uinque_keys == 0) {
            printf("not have any uinque keys!!\n");
        }
        if ((user_uinque_keys & DATA_ID) == DATA_ID) {
            printf("data_id\n");
        }
        if ((user_uinque_keys & USER_ID) == USER_ID) {
            printf("user_id\n");
        }
        if ((user_uinque_keys & DATA_PATH) == DATA_PATH) {
            printf("data_path\n");
        }
        if ((user_uinque_keys & DATA_SIZE) == DATA_SIZE) {
            printf("data_size!\n");
        }
        if ((user_uinque_keys & DATA_TYPE) == DATA_TYPE) {
            printf("data_type\n");
        }
        if ((user_uinque_keys & COMPUTER_ID) == COMPUTER_ID) {
            printf("data_computer_id\n");
        }
        printf("\n--------------------------------------\n\n");
    }

    return E_NOT_E;
}
int range_search_databse(Tables database,Flag table, int start, int end,int *users_id)  {
    if (table == USERS_TABLE)
    {
        if (end > database.database_settings.num_of_users) {
            end = database.database_settings.num_of_users;
        }

        int matches = 0;
        printf("\t\t\t range search from %d - %d\n", start, end);
        for (int id = start; id < end; id++) {
            if (database.users[id].user_id != DEFAULT_INT) {//found a user by the id i
                if (users_id != NULL) {
                    users_id[matches] = database.users[id].user_id;
                }
                matches++;
                char my_string[300];
                sprintf(my_string, "|user_id:%d |user_name: %s |user_pass: %s |user_email: %s |user_phon: %s |user_permission: %d |user_coputer: %d |\n", database.users[id].user_id, database.users[id].user_name, database.users[id].user_pass, database.users[id].user_email, database.users[id].user_phon, database.users[id].user_permission, database.users[id].user_computer);
                size_t length = strlen(my_string);
                print_line(length - 1);
                printf("%s", my_string);
                print_line(length - 1);
            }
            else {//not found a user by the id i
                printf("--------\n");
            }
            
        }
        if (matches < 0)
        {
            return E_NOT_F;
        }
        else {
            return matches;
        }
    }
    else if (table == USERS_DATA_TABLE) {
        if (end > database.database_settings.num_of_users_data) {
            end = database.database_settings.num_of_users_data;
        }

        int matches = 0;
        printf("\t\t\t range search from %d - %d\n", start, end);
        for (int id = start; id < end; id++) {
            if (database.users_data[database.database_settings.num_of_users_data].data_id != DEFAULT_INT) {//found a use4_data by the id i
                if (users_id != NULL) {
                    users_id[matches] = database.users_data[id].data_id;
                }
                matches++;
                char my_string[300];
                printf("\t\t\t\t table:%d user_data_id:%d\n", USERS_DATA_TABLE, id);
                sprintf(my_string, "|data_id:%d |user_id: %d |data_type: %d |data_size: %d |user_path: %s |coputer_id: %d |\n", database.users_data[id].data_id, database.users_data[id].user_id, database.users_data[id].data_type, database.users_data[id].data_size, database.users_data[id].data_path, database.users_data[id].computer_id);
                size_t length = strlen(my_string);
                print_line(length - 1);
                printf("%s", my_string);
                print_line(length - 1);
            }
            else//not found a user_data by the id i
            {
                printf("--------\n");
            }

        }
        if (matches < 0)
        {
            return E_NOT_F;
        }
        else {
            return matches;
        }
    }
    else {
        return E_N_EN_TA;
    }
    
}
int search_by_attribute(Tables database, Flag table, Flag attribute, void* data ,int * users_id) {
    int matches = 0;
    if (table == USERS_TABLE) {
        if (check_uinque(database, table, attribute) != E_NOT_E_U) {
            printf("\n\tsearch by uinque attribute \n\t----------------------\n\n");

            int id  = search_in_hash_table(&database, table, (char*)data);
            if (id != E_NOT_F) {
                printf("\tuinque user id:%d data:%s\n", id, (char*)data);
                printf("\n\t-----------end--------\n");
            }
            else {
                return E_NOT_F;
            }
        }
        else {
            printf("\n\tsearch by attribute\n\t----------------------\n\n");
            if (attribute_type(attribute) == 's') { // when attribute is a string
                char* search = (char*)data;
                for (int i = 0; i < database.database_settings.users_table_size-1; i++) {
                    switch (attribute) {
                    case USER_NAME:
                        if (strcmp(search, database.users[i].user_name) == 0) {
                            users_id[matches]=database.users[i].user_id;
                            printf("\tuser id:%d data:%s\n", database.users[i].user_id, search);
                            matches++;
                        }
                        break;
                    case USER_PASS:
                        if (strcmp(search, database.users[i].user_pass) == 0) {
                            users_id[matches] = database.users[i].user_id;
                            printf("\tuser id:%d data:%s\n", database.users[i].user_id, search);

                            matches++;
                        }
                        break;
                    case USER_EMAIL:
                        if (strcmp(search, database.users[i].user_email) == 0) {
                            users_id[matches] = database.users[i].user_id;
                            printf("\tuser id:%d data:%s\n", database.users[i].user_id, search);
                            matches++;
                        }
                        break;
                    case USER_PHON:
                        if (strcmp(search, database.users[i].user_phon) == 0) {
                            users_id[matches] = database.users[i].user_id;
                            printf("\tuser id:%d data:%s\n", database.users[i].user_id,search);
                            matches++;
                        }
                        break;

                    }
                }
                printf("\n\t-----------end--------\n");
                
            }
            else if (attribute_type(attribute) == 'i') { // when attribute is an int
                int search = (int*)data;
                for (int i = 0; i < database.database_settings.users_table_size-1; i++) {
                    switch (attribute) {
                    case USER_ID:
                        if (search == database.users[i].user_id) {
                            users_id[matches] = database.users[i].user_id;
                            printf("\tuser id:%d data:%d\n", database.users[i].user_id,search);
                            matches++;
                        }
                        break;
                    case USER_COMPUTER:
                        if (search == database.users[i].user_computer) {
                            users_id[matches] = database.users[i].user_id;
                            printf("\tuser id:%d data:%d\n", database.users[i].user_id, search);
                            matches++;
                        }
                        break;
                    case USER_PERMISSION:
                        if (search == database.users[i].user_permission) {
                            users_id[matches] = database.users[i].user_id;
                            printf("\tuser id:%d data:%d\n", database.users[i].user_id, search);
                            matches++;
                        }
                        break;

                    }
                }
                printf("\n\t-----------end--------\n");
            }
            else {
                return E_INVALID_ATTRIBUTE;
            }
        }
    }
    else if (table == USERS_DATA_TABLE) {
        if (check_uinque(database, table, attribute) == 1) {
            printf("Attribute is unique in USERS_DATA_TABLE.\n");
            return E_NOT_E;
        }
        else {
            if (attribute_type(attribute) == 's') { // when attribute is a string
                char* search = (char*)data;
                for (int i = 0; i < database.database_settings.users_data_table_size-1; i++) {
                    switch (attribute) {
                    case DATA_PATH:
                        if (strcmp(search, database.users_data[i].data_path) == 0) {
                            users_id[matches] = database.users_data[i].data_id;
                            printf("data id:%d user id:%d\n", database.users_data[i].data_id, database.users_data[i].user_id);
                            matches++;
                            
                        }
                        break;

                    }
                }
            }
            else if (attribute_type(attribute) == 'i') { // when attribute is an int
                int* search = (int*)data;
                for (int i = 0; i < database.database_settings.users_data_table_size-1; i++) {
                    switch (attribute) {
                    case DATA_ID:
                        if (*search == database.users_data[i].data_id) {
                            users_id[matches] = database.users_data[i].data_id;
                            printf("data id:%d user id:%d\n", database.users_data[i].data_id, database.users_data[i].user_id);
                            matches++;
                        }
                        break;
                    case DATA_SIZE:
                        if (*search == database.users_data[i].data_size) {
                            users_id[matches] = database.users_data[i].data_id;
                            printf("data id:%d user id:%d\n", database.users_data[i].data_id, database.users_data[i].user_id);
                            matches++;
                        }
                        break;
                    case DATA_TYPE:
                        if (*search == database.users_data[i].data_type) {
                            users_id[matches] = database.users_data[i].data_id;
                            printf("data id:%d user id:%d\n", database.users_data[i].data_id, database.users_data[i].user_id);
                            matches++;
                        }
                        break;
                        
                    }
                }


            }
            else {
                return E_INVALID_ATTRIBUTE;
            }
        }
    }
    else {
        return E_N_EN_TA; // Return error for invalid table
    }
    if (matches < 0) {
        return E_NOT_F;
    }
    else {
        return matches; // Return a default success value if no errors

    }
}
int check_primary(Tables   database,Flag table, Flag attribute) {
    if (table == USERS_TABLE) {
        if ((database.user_primary_keys & attribute) == attribute) {
            return 1;
        }
        else {
            return E_NOT_E_P;
        }
    }else if (table == USERS_DATA_TABLE) {
        if ((database.user_data_primary_keys & attribute) == attribute) {
            return 1;
        }
        else {
            return E_NOT_E_P;
        }
    }
    else
    {
        return E_N_EN_TA;
    }
    return E_NOT_F;
}
int check_uinque(Tables database, Flag table, Flag attribute) {
    if (table == USERS_TABLE) {
        if ((database.user_uinque_keys & attribute) == attribute) {
            return 1;
        }
        else {
            return E_NOT_E_U;
        }
    }
    else if (table == USERS_DATA_TABLE) {
        if ((database.user_data_uinque_keys & attribute) == attribute) {
            return 1;
        }
        else {
            return E_NOT_E_U;
        }
    }
    else
    {
        return E_N_EN_TA;
    }
    return E_NOT_F;
}
int realloc_database(Tables* database, Flag table, int size) {
    printf("in realloc!\n");
    if (table == USERS_TABLE || table == ALLTABLES) {
        // Reallocate memory for users
        Users* res = realloc(database->users, sizeof(database->users[0]) * size);
        if (res == NULL) {
            fprintf(stderr, "Failed to reallocate memory for users\n");
            return E_MAM_E;
        }
        database->users = res;

        // Reallocate memory for user_id_list
        char* res_char = realloc(database->database_settings.user_id_list, sizeof(char) * size);
        if (res_char == NULL) {
            fprintf(stderr, "Failed to reallocate memory for user_id_list\n");
            return E_MAM_E;
        }
        database->database_settings.user_id_list = res_char;

        // Initialize new entries in user_id_list to '0'
        for (int i = database->database_settings.users_table_size; i < size; i++) {
            database->database_settings.user_id_list[i] = '0';
        }

        // Update the table size
        database->database_settings.users_table_size = size;
    }
    else if (table == USERS_DATA_TABLE || table == ALLTABLES) {
        // Reallocate memory for users_data
        Users_data* res = realloc(database->users_data, sizeof(database->users_data[0]) * size);
        if (res == NULL) {
            fprintf(stderr, "Failed to reallocate memory for users_data\n");
            return E_MAM_E;
        }
        database->users_data = res;

        // Reallocate memory for user_data_id_list
        char* res_char = realloc(database->database_settings.user_data_id_list, sizeof(char) * size);
        if (res_char == NULL) {
            fprintf(stderr, "Failed to reallocate memory for user_data_id_list\n");
            return E_MAM_E;
        }
        database->database_settings.user_data_id_list = res_char;

        // Initialize new entries in user_data_id_list to '0'
        for (int i = database->database_settings.users_data_table_size; i < size; i++) {
            database->database_settings.user_data_id_list[i] = '0';
        }

        // Update the table size
        database->database_settings.users_data_table_size = size;
    }
    else {
        return E_N_EN_TA;  // Invalid table
    }

    return E_NOT_E;  // Success
}

int get_primary_id(Tables database, Flag table) {

    if (table == USERS_TABLE) {
        for (int i = 0; i < database.database_settings.users_table_size; i++) {
            if (database.database_settings.user_id_list[i] == '0') {
                database.database_settings.user_id_list[i] = '1';  // Mark the ID as used
                return i;  // Return the available ID
            }
        }
    }
    else if (table == USERS_DATA_TABLE) {
        for (int i = 0; i < database.database_settings.users_data_table_size; i++) {
            if (database.database_settings.user_data_id_list[i] == '0') {
                database.database_settings.user_data_id_list[i] = '1';  // Mark the ID as used
                return i;
            }
        }
    }
    else {
        return E_N_EN_TA;
    }
    return E_NOT_F;  
}

int check_uniqueness(Tables* database, Flag attribute, char* data){
    int attribute_hash_table;
    switch (attribute) {
    case USER_NAME:
        attribute_hash_table = HASH_TABLE_NAME;
        break;
    case USER_PASS:
        attribute_hash_table = HASH_TABLE_PASS;
        break;
    case USER_EMAIL:
        attribute_hash_table = HASH_TABLE_EMAIL;
        break;
    case USER_PHON:
        attribute_hash_table = HASH_TABLE_PHON;
        break;
    default:
        return E_NOT_E_U;
        break;
    }
    if (search_in_hash_table(database, attribute_hash_table, data) < 0) {
        return E_SUCCESS;
    }
    return E_NOT_E_U;
        
}
//#define E_MAM_E -1 //not can malloc memory error
//#define E_N_EN_TA -2 // not enter table
//#define E_EN_IV_IN -3 //enter invalid input
//#define E_INVALID_ATTRIBUTE -4
//#define E_NOT_E -5 //not error
//#define E_NOT_F -6 // not found!
//#define E_NOT_E_U -7 // not enter uinque
//#define E_NOT_E_P -8 //not enter primary
//#define E_OP_FILE -9 //error opening file
void log_error(Error_struct* error_info, const char* file_path) {
    FILE* file = fopen(file_path, "a");  // Open in append mode
    if (file == NULL) {
        printf("Error opening log file.\n");
        return;
    }

    // Format and write the error information to the file
    fprintf(file, "Error Time: %[^|] | Error: %d | Function: %[^|] | Message: %[^|]\n",
        error_info->error_time,  // Now correctly handles the string format for time
        error_info->error,
        error_info->error_function,
        error_info->error_message);


    fclose(file);
    printf("Error logged successfully.\n");
}
int handle_error(int error_code, const char* function, const char* message) {
    if (error_code == E_NOT_E) {
        return 0;  // No error, return early
    }

    Error_struct error_info;
    time_t current_time;
    struct tm* time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(error_info.error_time, sizeof(error_info.error_time), "%Y-%m-%d %H:%M:%S", time_info);

    error_info.error = error_code;
    strncpy(error_info.error_function, function, sizeof(error_info.error_function) - 1);
    error_info.error_function[sizeof(error_info.error_function) - 1] = '\0';  
    strncpy(error_info.error_message, message, sizeof(error_info.error_message) - 1);
    error_info.error_message[sizeof(error_info.error_message) - 1] = '\0'; 

    log_error(&error_info, "error_log.txt");

    return error_code;
}

int clean_file(const char* file_path) {
    FILE* file = fopen(file_path, "w");  
    if (file == NULL) {
        printf("Error: Unable to open the file.\n");
        return E_OP_FILE;
    }

    fclose(file);
    printf("File cleaned successfully.\n");
    return E_NOT_E; 
}
int get_last_error(const char* file_path, Error_struct* last_error) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Error opening log file.\n");
        return E_OP_FILE;
    }

    char line[256]; 
    char last_line[256] = { 0 };  

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) > 1) { 
            strncpy(last_line, line, sizeof(last_line) - 1);
        }
    }

    fclose(file);

    if (strlen(last_line) == 0) {
        printf("No errors found in the log file.\n");
        return E_EMPTY_FILE; 
    }
    printf("%s\n", last_line);
    sscanf(last_line, "Error Time: %s | Error: %d | Function: %s | Message: %s\n",
        last_error->error_time,
        &last_error->error,
        last_error->error_function,
        last_error->error_message);

    printf("Last error:\n"); 
    printf("Error Time: %s\n", last_error->error_time);
    printf("Error Code: %d\n", last_error->error);
    printf("Error Function: %s\n", last_error->error_function);
    printf("Error Message: %s\n", last_error->error_message);

    return E_NOT_E;  
}

int add_manager(Tables* database, Managers manager) {
    int i;
    for (i = 0; i < NUM_OF_MANAGERS;i++)
    {
        
        if (database->managers_table[i].manager_id == DEFAULT_INT) {
            database->managers_table[i].manager_id = manager.manager_id;
            database->managers_table[i].manager_permission = manager.manager_permission;
            strcpy(database->managers_table[i].manager_name, manager.manager_name);
            strcpy(database->managers_table[i].manager_pass, manager.manager_pass);
            strcpy(database->managers_table[i].manager_email, manager.manager_email);
            strcpy(database->managers_table[i].manager_phon, manager.manager_phon);
            return E_NOT_E;
        }
        
        
    }
    if (database->managers_table[i].manager_id!= manager.manager_id)
    {
        printf("all the managers solts in use !\n");
        return E_R_O_S;
    }
    return E_NOT_E;
}

int init_managers(Tables* database) {
    for (int i = 0; i < NUM_OF_MANAGERS; i++)
    {
        database->managers_table[i].manager_id = DEFAULT_INT;
        database->managers_table[i].manager_permission = DEFAULT_INT;
        strcpy(database->managers_table[i].manager_name, DEFAULT_STRING);
        strcpy(database->managers_table[i].manager_pass, DEFAULT_STRING);
        strcpy(database->managers_table[i].manager_email, DEFAULT_STRING);
        strcpy(database->managers_table[i].manager_phon, DEFAULT_STRING);
    }
    return E_NOT_E;
}