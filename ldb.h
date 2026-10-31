/*
    * Name: [ldb]
    * Description: [Single header, lightweight, local database]
    * Author: [dartxx | kerrer1234567890@gmail.com]
    * Date: [2026-10-31]
*/


#ifndef LDB_INCLUDE_H_
#define LDB_INCLUDE_H_

#include <stdio.h>
#include <stdint.h>

typedef struct LDB LDB;

const char *ldb_get_error();

int ldb_init(const char *output_dir, LDB *ldb);
void ldb_close(LDB *ldb);

int ldb_add_int(const LDB *ldb, const char *key, int32_t value);
int ldb_get_int(const LDB *ldb, const char *key, int32_t *data);

#endif // LDB_INCLUDE_H_

// To add implementation in your file, you need to define LDB_IMPLEMENTATION macro.
// The implementation must come first.
//     #define LDB_IMPLEMENTATION
//     #include "ldb.h"

#ifdef LDB_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define DB_FILE_NAME "/db.ldb"
#define DB_FILE_NAME_LEN (sizeof(DB_FILE_NAME) - 1)

typedef struct LDB {
    char *file_path;
} LDB;

#define LDB_ERROR_LEN 128

static char ldb_error[LDB_ERROR_LEN];

#define LDB_SET_ERROR(...) do { snprintf(ldb_error, LDB_ERROR_LEN, __VA_ARGS__); } while (0)

const char *ldb_get_error() {
    return ldb_error;
}

static FILE *open_file(const char *file_path, const char *mode) {
    errno = 0;
    FILE *f = fopen(file_path, mode);
    if (!f) {
        LDB_SET_ERROR("failed to open file: %s\n", strerror(errno));
    }
    return f;
}

static bool is_file_exists(const char *file_path) {
    FILE *f = fopen(file_path, "rb");
    if (!f) {
        return false;
    }

    fclose(f);
    return true;
}

static bool create_database_file(const char *file_path) {
    errno = 0;
    FILE *f = fopen(file_path, "wb");
    if (!f) {
        LDB_SET_ERROR("failed to create new database file: %s\n", strerror(errno));
        return false;
    }

    fclose(f);
    return true;
}

int ldb_init(const char *output_dir, LDB *ldb) {
    if (!output_dir) {
        LDB_SET_ERROR("output_dir is NULL\n");
        return 1;
    }
    
    const size_t output_dir_len = strlen(output_dir);

    char *file_path = malloc(output_dir_len + DB_FILE_NAME_LEN + 1);
    if (!file_path) {
        LDB_SET_ERROR("failed to allocate memory");
        return 1;
    }

    memcpy(file_path, output_dir, output_dir_len);
    strcpy(file_path + output_dir_len, DB_FILE_NAME);

    if (is_file_exists(file_path)) {
        ldb->file_path = file_path;
        return 0;
    }
    
    if (!create_database_file(file_path)) {
        return 1;
    }

    ldb->file_path = file_path;
    return 0;
}

void ldb_close(LDB *ldb) {
    if (!ldb || !ldb->file_path) {
        return;
    }
    free(ldb->file_path);
    ldb->file_path = NULL;
}

#define DB_DELIMITER "="
#define DB_DELIMITER_LEN (sizeof(DB_DELIMITER) - 1)

static bool write_key_and_eqaul(const char *key, FILE *f) {
    const size_t key_len = strlen(key);
    size_t n = fwrite(key, sizeof(char), key_len, f);
    if (key_len != n) {
        LDB_SET_ERROR("failed to write key\n");
        return false;
    }

    n = fwrite("=", sizeof(char), DB_DELIMITER_LEN, f);
    if (DB_DELIMITER_LEN != n) {
        LDB_SET_ERROR("failed to write delimiter\n");
        return false;
    }
    return true;
}

int ldb_add_int(const LDB *ldb, const char *key, int32_t value) {
    FILE *f = open_file(ldb->file_path, "ab");
    if (!f) return 1;
    write_key_and_eqaul(key, f);
    if (fwrite(&value, sizeof(value), 1, f) != 1) {
        LDB_SET_ERROR("failed to write integer\n");
    }
    fputc('\n', f);
    fclose(f);
    return 0;
}

//#define LDB_MAX_LINE_LEN 128
static constexpr size_t ldb_max_line_len =  128;

int ldb_get_int(const LDB *ldb, const char *key, int32_t *data) {
    char line[ldb_max_line_len];
    FILE *f = open_file(ldb->file_path, "rb");
    if (!f) return 1;

    fread(&line, sizeof(char), ldb_max_line_len, f);
        printf("%s\n", line);
        /*const char *found = strstr(line, DB_DELIMITER);
        size_t key_len = found - line;
        char founded_key[key_len + 1];
        memcpy(founded_key, line, key_len);
        founded_key[key_len] = '\0';
        if (strcmp(key, founded_key) == 0) {
            printf("our data target - %" PRId32 "\n", ++found);
        }*/
    
    fclose(f);
    return 0;
}

#endif // LDB_IMPLEMENTATION_
