/*
    * Name: [ldb]
    * Description: [Single header, lightweight, local database]
    * Author: [dartxx | kerrer1234567890@gmail.com]
    * Date: [2026-10-31]
*/


#ifndef LDB_INCLUDE_H_
#define LDB_INCLUDE_H_

int ldb_init(const char *file_location);

#endif // LDB_INCLUDE_H_

// To add implementation in your file, you need to define LDB_IMPLEMENTATION macro.
// The implementation must come first.
//     #define LDB_IMPLEMENTATION
//     #include "ldb.h"

#ifdef LDB_IMPLEMENTATION

#include <stdio.h>

int ldb_init(const char *file_location) {
    printf("Hello, %s\n", file_location);
    return 0;
}

#endif // LDB_IMPLEMENTATION_
