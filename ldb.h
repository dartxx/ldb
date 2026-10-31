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
int ldb_add_string(const LDB *ldb, const char *key, const char *value);

int ldb_get(const LDB *ldb, const char *key, void *data);

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

#define LDB_FILE_NAME "/db.ldb"
#define LDB_FILE_NAME_LEN (sizeof(LDB_FILE_NAME) - 1)

#define LDB_DELIMITER "="
#define LDB_DELIMITER_LEN (sizeof(LDB_DELIMITER) - 1)

typedef struct LDB {
    char *file_path;
} LDB;

#define LDB_ERROR_LEN 128

static char ldb_error[LDB_ERROR_LEN];

#define LDB_SET_ERROR(...) do { snprintf(ldb_error, LDB_ERROR_LEN, __VA_ARGS__); } while (0)

const char *ldb_get_error() {
    return ldb_error;
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

    char *file_path = malloc(output_dir_len + LDB_FILE_NAME_LEN + 1);
    if (!file_path) {
        LDB_SET_ERROR("failed to allocate memory");
        return 1;
    }

    memcpy(file_path, output_dir, output_dir_len);
    strcpy(file_path + output_dir_len, LDB_FILE_NAME);

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

#define CHECK_FILE_OPEN(f) do { if (!(f)) { LDB_SET_ERROR("failed to open file\n"); return 1; } } while (0)

static int ldb_add(const LDB *ldb, const char *key, const void *data, size_t size) {
    FILE *f = fopen(ldb->file_path, "ab");
    CHECK_FILE_OPEN(f);

    // const size_t key_len = strlen(key);
    // if (fwrite(key, sizeof(char), key_len, f) != key_len) {
    //     LDB_SET_ERROR("failed to write key\n");
    //     fclose(f);
    //     return 1;
    // }

    // if (fwrite(LDB_DELIMITER, sizeof(char), LDB_DELIMITER_LEN, f) != LDB_DELIMITER_LEN) {
    //     LDB_SET_ERROR("failed to write delimiter\n");
    //     fclose(f);
    //     return 1;
    // }

    if (size == 0 || fwrite(data, 1, size, f) != size) {
        LDB_SET_ERROR("failed to write value\n");
        fclose(f);
        return 1;
    }

    // if (fputc('\n', f) == EOF) {
    //     LDB_SET_ERROR("failed to write new line character\n");
    //     fclose(f);
    //     return 1;
    // }

    fclose(f);
    return 0;
}

int ldb_add_int(const LDB *ldb, const char *key, int32_t value) {
    return ldb_add(ldb, key, &value, sizeof(value));
}

int ldb_add_string(const LDB *ldb, const char *key, const char *value) {
    return ldb_add(ldb, key, value, strlen(value));
}

// TODO: try to fwrite structure and read it

int ldb_get(const LDB *ldb, const char *key, void *data) {
    FILE *f = fopen(ldb->file_path, "rb");
    CHECK_FILE_OPEN(f);

    int num;

    fread(&num, 1, sizeof(num), f);
    
    printf("%d\n", num);

    fclose(f);
    return 0;
}

#endif // LDB_IMPLEMENTATION_
