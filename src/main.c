#include <stdio.h>
#include "cpu.h"

int main() {
    cpu_t cpu = cpu_create();
    cpu_print_status(&cpu);
    return 0;
}