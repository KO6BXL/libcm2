#include <stdio.h>
#include <stdlib.h>

#include "../cm2.h"

int main() {
    collection *col = new_collection();

    append(block_and(), col);
    unsigned long nor_block = append(block_nor() ,col);
    mod_offset(nor_block, col, (offset){1, 0, 1});

    char *string = compile(col);
    printf("%s\n", string);
}