#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int main() {
    struct Stack stk1;

    STACK_CTOR(stk1, default_size);

    StackPush(&stk1, 1);
    StackPush(&stk1, 2);
    StackPush(&stk1, 3);
    CHECK_HASH_SUM(stk1);

    StackDtor(&stk1);
    //StackPush(&stk1, 7);
    //StackPush(&stk1, 8);

return 0;
}




