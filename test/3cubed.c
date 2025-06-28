#include <stdio.h>

#include "../cm2.h"

int main() {
    collection *col = new_collection();
    for (int x = 0; x < 100; x++) {
        for (int y = 0; y < 100; y++) {
            for (int z = 0; z < 100; z++) {
                unsigned long and = append(block_and(), col);
                mod_offset(and, col, (offset) {x,y,z});
            }
        }
        printf("X: %d\n", x);
    }
    char *str = compile(col);
    printf("%s\n", str);
}
