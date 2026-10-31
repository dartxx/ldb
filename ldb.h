/*
    * Name: [ldb]
    * Description: [Single header, lightweight, local database]
    * Author: [dartxx | kerrer1234567890@gmail.com]
    * Date: [2026-10-31]
*/


#ifndef LDB_INCLUDE_H_
#define LDB_INCLUDE_H_

int ldb_init(const char *output_dir);

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

#define DB_FILE_NAME "/db.ldb"
#define DB_FILE_NAME_LEN (sizeof(DB_FILE_NAME) - 1)

#define ALLOCATION_FAIL do { fprintf(stderr, "failed to allocate memory\n"); return 1; } while(0)

int ldb_init(const char *output_dir) {
    if (!output_dir) {
        fprintf(stderr, "output_dir is NULL\n");
        return 1;
    }
    
    const size_t output_dir_len = strlen(output_dir);
    char *full_path = malloc(output_dir_len + DB_FILE_NAME_LEN + 1);
    if (!full_path) {
        ALLOCATION_FAIL;
    }

    memcpy(full_path, output_dir, output_dir_len);
    strcpy(full_path + output_dir_len, DB_FILE_NAME);
    
    FILE *f = fopen(full_path, "rb");
    if (f == NULL) {
        errno = 0;
        FILE *nf = fopen(full_path, "wb");
        if (nf == NULL) {
            fprintf(stderr, "failed to create new database file: %s\n", strerror(errno));
            free(full_path);
            return 1;
        }
        printf("new file added successfully!\n");
    } else {
        printf("file db.ldb already exists\n");
        fclose(f);
    }

    free(full_path);
    return 0;
}

#endif // LDB_IMPLEMENTATION_
