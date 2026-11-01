#define LDB_IMPLEMENTATION
#include "ldb.h"

#define LDB_INT_TEST(ldb, key, value) do { if (ldb_add_int(&ldb, key, value) != 0) { fprintf(stderr, ldb_get_error()); return 1; } } while (0)

int main() {
    LDB ldb;
    
    if (ldb_init("./", &ldb) != 0) {
        fprintf(stderr, ldb_get_error());
        return 1;
    }

    // ldb_add_string(&ldb, "tuzz", "val");
    
    // LDB_INT_TEST(ldb, "fizz", 123);
    // LDB_INT_TEST(ldb, "tuzz", 1488);

    int32_t data;
    ldb_get(&ldb, "tuzz", &data);
    
    ldb_close(&ldb);
    return 0;
}
