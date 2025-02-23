#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/parse.h"
#include "py/compile.h"
#include "py/obj.h"

extern void create_and_print_list();  // Assume this is defined in the Cython-generated code

int main() {
    mp_init();  // Initialize MicroPython
    create_and_print_list();  // Call the Cython-compiled function
    return 0;
}