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

#define LDB_FILE_NAME "/db.ldb"
#define LDB_FILE_NAME_LEN (sizeof(LDB_FILE_NAME) - 1)

#define LDB_KEY_MAX_LEN 64
#define LDB_STRING_VALUE_MAX_LEN 128

typedef struct {
    char *file_path;
} LDB;

typedef enum {
    LDB_OK          = 0,
    LDB_ERR_NOMEM   = 1,
    LDB_ERR_INVINP  = 2,
    LDB_ERR_IO      = 3,
} LDB_Error;

const char *ldb_get_error();
void ldb_clear_error();

LDB_Error ldb_init(const char *output_dir, LDB *ldb);
LDB_Error ldb_free(LDB *ldb);

LDB_Error ldb_add_int(const LDB *ldb, const char *key, int32_t value);
LDB_Error ldb_add_string(const LDB *ldb, const char *key, const char *value);

LDB_Error ldb_get(const LDB *ldb, const char *key, void *dst, size_t size);

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

#define LDB_ERROR_MAX_LEN 128

static char ldb_error[LDB_ERROR_MAX_LEN];

#define LDB_SET_ERROR(...) do { snprintf(ldb_error, LDB_ERROR_MAX_LEN, __VA_ARGS__); } while (0)

const char *ldb_get_error() {
    return ldb_error;
}

void ldb_clear_error() {
    ldb_error[0] = '\0';
}

LDB_Error ldb_init(const char *output_dir, LDB *ldb) {
    if (!output_dir) {
        LDB_SET_ERROR("output_dir is NULL\n");
        return LDB_ERR_INVINP;
    }
    
    const size_t output_dir_len = strlen(output_dir);

    char *file_path = malloc(output_dir_len + LDB_FILE_NAME_LEN + 1);
    if (!file_path) {
        LDB_SET_ERROR("failed to allocate memory\n");
        return LDB_ERR_NOMEM;
    }

    memcpy(file_path, output_dir, output_dir_len);
    strcpy(file_path + output_dir_len, LDB_FILE_NAME); // strcpy add '\0'

    {
        FILE *f = fopen(file_path, "rb");
        if (f) {
            fclose(f);
            ldb->file_path = file_path;
            return LDB_OK;
        }
    }

    {
        FILE *f = fopen(file_path, "wb");
        if (!f) {
            free(file_path);
            LDB_SET_ERROR("failed to create new database file\n");
            return LDB_ERR_IO;
        }
        fclose(f);
        ldb->file_path = file_path;
        return LDB_OK;
    }
}

LDB_Error ldb_free(LDB *ldb) {
    if (!ldb || !ldb->file_path) {
        LDB_SET_ERROR("empty LDB provided to ldb_free\n");
        return LDB_ERR_INVINP;
    }
    free(ldb->file_path);
    ldb->file_path = NULL;
    return LDB_OK;
}

#define CHECK_FILE_OPEN(f) do { if (!(f)) { LDB_SET_ERROR("failed to open file\n"); return LDB_ERR_IO; } } while (0)

static bool ldb_record_exists(const LDB *ldb, const LDB_Record *record) {
    FILE *f = fopen(ldb->file_path, "rb");
    CHECK_FILE_OPEN(f);
    
    LDB_Record founded_record;
    
    while (fread(&founded_record, sizeof(founded_record), 1, f) == 1) {
        if (strcmp(record->key, founded_record.key) == 0) goto exist;
    }
    fclose(f);
    return false;
    
exist:
    fclose(f);
    return true;
}

static LDB_Error ldb_add(const LDB *ldb, const LDB_Record *record) {
    if (ldb_record_exists(ldb, record)) {
        return LDB_OK;
    }
    
    FILE *f = fopen(ldb->file_path, "ab");
    CHECK_FILE_OPEN(f);

    if (fwrite(record, sizeof(*record), 1, f) != 1) {
        LDB_SET_ERROR("failed to write record to database file\n");
        fclose(f);
        return LDB_ERR_IO;
    }

    fclose(f);
    return LDB_OK;
}

LDB_Error ldb_add_int(const LDB *ldb, const char *key, int32_t value) {
    LDB_Record r = {0};
    LDB_RECORD_ADD_KEY(r, key);
    r.type = LDB_TYPE_INT;
    r.value.i = value;
    return ldb_add(ldb, &r);
}

LDB_Error ldb_add_string(const LDB *ldb, const char *key, const char *value) {
    const size_t value_len = strlen(value);
    if (value_len > LDB_STRING_VALUE_MAX_LEN) {
        LDB_SET_ERROR("max len of string in value is %d\n", LDB_STRING_VALUE_MAX_LEN);
        return LDB_ERR_IO;
    }
    
    LDB_Record r = {0};
    LDB_RECORD_ADD_KEY(r, key);
    r.type = LDB_TYPE_STRING;
    memcpy(r.value.s, value, value_len + 1);
    return ldb_add(ldb, &r);
}

LDB_Error ldb_get(const LDB *ldb, const char *key, void *dst, size_t size) {
    FILE *f = fopen(ldb->file_path, "rb");
    CHECK_FILE_OPEN(f);

    LDB_Record record;

    while (fread(&record, sizeof(record), 1, f) == 1) {
        if (strcmp(record.key, key) != 0) continue;
        
        switch (record.type) {
        case LDB_TYPE_INT:
            if (size < sizeof(record.value.i)) {
                LDB_SET_ERROR("buffer is too small. Required: %zu\n", sizeof(record.value.i));
                fclose(f);
                return LDB_ERR_INVINP;
            }
            memcpy(dst, &record.value.i, sizeof(record.value.i));
            goto end;
        
        case LDB_TYPE_STRING:
            const size_t value_len = strlen(record.value.s);
            if (size < value_len + 1) {
                LDB_SET_ERROR("buffer is too small. required - %zu\n", value_len + 1);
                fclose(f);
                return LDB_ERR_INVINP;
            }
            memcpy(dst, record.value.s, value_len + 1);
            goto end;
        }
    }

end:
    fclose(f);
    return LDB_OK;
}

#endif // LDB_IMPLEMENTATION_
