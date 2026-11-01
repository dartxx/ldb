#define LDB_IMPLEMENTATION
#include "ldb.h"

int main() {
    LDB ldb;
    
    if (ldb_init("./", &ldb) != LDB_OK) {
        fprintf(stderr, ldb_get_error());
        return 1;
    }

    ldb_add_string(&ldb, "name", "Ivan");
    ldb_add_int(&ldb, "age", 16);
    
    int32_t age;
    ldb_get(&ldb, "age", &age, sizeof(age));
    char name[24];
    ldb_get(&ldb, "name", name, 24);
    
    printf("name - %s, age - %d\n", name, age);
    
    ldb_free(&ldb);
    return 0;
}
