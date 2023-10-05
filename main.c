#include "macros.h"
#include "utils/utils.h"
#include "parts/parts.h"


int main(int argc, char **argv) {
    char* file_name = NULL;
    if (argc == 2) {
        file_name = argv[1];
    }
    file_t* file = file_init();
    if (file == NULL) {
        return 99;
    }
    if (read_file(file_name, file) != 0) {
        file_dtor(file);
        return 99;
    }
    print_file(file);
    file_dtor(file);
    return 0;
}