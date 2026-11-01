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

typedef struct {
    char *file_path;
} LDB;

const char *ldb_get_error();

int ldb_init(const char *output_dir, LDB *ldb);
void ldb_close(LDB *ldb);

int ldb_add_int(const LDB *ldb, const char *key, int32_t value);
int ldb_add_string(const LDB *ldb, const char *key, const char *value);

// In this function, if you getting string, you should provide size of string
// It must be more than LDB_STRING_VALUE_MAX_LEN

int ldb_get(const LDB *ldb, const char *key, void *data, size_t size);

#endif // LDB_INCLUDE_H_

// To add implementation in your file, you need to define LDB_IMPLEMENTATION macro.
// The implementation must come first.
//     #define LDB_IMPLEMENTATION
//     #include "ldb.h"

#ifdef LDB_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define LDB_FILE_NAME "/db.ldb"
#define LDB_FILE_NAME_LEN (sizeof(LDB_FILE_NAME) - 1)

#define LDB_KEY_MAX_LEN 64
#define LDB_STRING_VALUE_MAX_LEN 128

typedef enum {
    LDB_TYPE_INT,
    LDB_TYPE_STRING,
} LDB_RecordType;

typedef struct {
    char key[LDB_KEY_MAX_LEN + 1];
    LDB_RecordType type;
    union {
        int32_t i;
        char    s[LDB_STRING_VALUE_MAX_LEN + 1];
    } value;
} LDB_Record;

#define LDB_RECORD_ADD_KEY(r, key) do {                                              \
    const size_t key_len = strlen(key);                                              \
    if (key_len > LDB_KEY_MAX_LEN) {                                                 \
        LDB_SET_ERROR("key length must not exceed %d characters", LDB_KEY_MAX_LEN);  \
        return 1;                                                                    \
    }                                                                                \
    memcpy(r.key, key, key_len + 1);                                                 \
} while(0)

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
        free(file_path);
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

static int ldb_add(const LDB *ldb, LDB_Record *record) {
    FILE *f = fopen(ldb->file_path, "ab");
    CHECK_FILE_OPEN(f);

    if (fwrite(record, sizeof(*record), 1, f) != 1) {
        LDB_SET_ERROR("failed to write record to db file\n");
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}

int ldb_add_int(const LDB *ldb, const char *key, int32_t value) {
    LDB_Record r = {0};
    LDB_RECORD_ADD_KEY(r, key);
    r.type = LDB_TYPE_INT;
    r.value.i = value;
    return ldb_add(ldb, &r);
}

int ldb_add_string(const LDB *ldb, const char *key, const char *value) {
    if (!ldb || !ldb->file_path || !key || !value) {
        LDB_SET_ERROR("invalid argument");
        return 1;
    }
    
    const size_t value_len = strlen(value);
    if (value_len > LDB_STRING_VALUE_MAX_LEN) {
        LDB_SET_ERROR("max len of string in value is %d\n", LDB_STRING_VALUE_MAX_LEN);
        return 1;
    }
    LDB_Record r = {0};
    LDB_RECORD_ADD_KEY(r, key);
    r.type = LDB_TYPE_STRING;
    memcpy(r.value.s, value, value_len + 1);
    return ldb_add(ldb, &r);
}

int ldb_get(const LDB *ldb, const char *key, void *data, size_t size) {
    FILE *f = fopen(ldb->file_path, "rb");
    CHECK_FILE_OPEN(f);

    LDB_Record record;

    while (fread(&record, sizeof(record), 1, f) == 1) {
        if (strcmp(record.key, key) != 0) continue;
        
        switch (record.type) {
        case LDB_TYPE_INT:
            memcpy(data, &record.value.i, sizeof(record.value.i));
            goto end;
        
        case LDB_TYPE_STRING:
            if (size < LDB_STRING_VALUE_MAX_LEN + 1) {
                LDB_SET_ERROR("buffer is too small. minimum - %d\n", LDB_STRING_VALUE_MAX_LEN + 1);
                fclose(f);
                return 1;
            }
            memcpy(data, record.value.s, sizeof(record.value.s));
            goto end;
        }
    }

    fclose(f);
    return 1;

end:
    fclose(f);
    return 0;
}

#endif // LDB_IMPLEMENTATION_
