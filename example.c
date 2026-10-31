#define LDB_IMPLEMENTATION
#include "ldb.h"

#define PRINT_LDB_ERROR fprintf(stderr, ldb_get_error())

int main() {
    LDB ldb;
    
    int s = ldb_init("./", &ldb);
    if (s != 0) {
        PRINT_LDB_ERROR;
        return 1;
    }

    if (ldb_add_int(&ldb, "key", 123) != 0) {
        PRINT_LDB_ERROR;
        return 1;
    }

    ldb_close(&ldb);
    return 0;
}
