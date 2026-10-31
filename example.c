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
    if (ldb_add_int(&ldb, "fizz", 314) != 0) {
        PRINT_LDB_ERROR;
        return 1;
    }
    if (ldb_add_int(&ldb, "tuzz", 1488) != 0) {
        PRINT_LDB_ERROR;
        return 1;
    }
    if (ldb_add_int(&ldb, "buzz", 67) != 0) {
        PRINT_LDB_ERROR;
        return 1;
    }
    if (ldb_add_int(&ldb, "kazan", 76) != 0) {
        PRINT_LDB_ERROR;
        return 1;
    }

    int32_t data;
    ldb_get_int(&ldb, "tuzz", &data);

    ldb_close(&ldb);
    return 0;
}
