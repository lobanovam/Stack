#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stack.h"
#include "log.h"


void StackDtor(struct Stack *stk);
ULL calcGNUhash(void* start_ptr, int num_bytes);

void printStack(int *data, size_t capacity);
void printStack(double *data, size_t capacity);
void printStack(char *data, size_t capacity);

int getPoison(int);
double getPoison(double);
size_t getPoison(size_t);
char getPoison(char);
char *getPoison(char*);

Elem_t StackPop(struct Stack *stk);
void StackPush(struct Stack *stk, Elem_t value);
void StackRecalloc(struct Stack *stk, size_t ssize);

unsigned long StackError(struct Stack *stk);
void printErr(unsigned long error);

int checkHash(struct Stack *stk);

struct Description{
    char *name;
    char *func;
    int line;
    char *file;
};

struct Stack{
    ULL stack_canary_l;
    ULL* data_canary_l;
    Elem_t* data;
    ULL* data_canary_r;
    size_t ssize;
    size_t capacity;
    struct Description stk_disc;
    ULL errors;
    ULL stack_canary_r;
    ULL data_hash_sum;
    ULL stack_hash_sum;
};

static char *ErrorNames[] = {
    "data has NULL pointer",//0
    "invalid value of stack size",//1
    "invalid capacity of stack size",//2
    "size larger then capacity",//3
    "stack left canary error",//4
    "stack right canary error",//5
    "data left canary error",//6
    "data right canary error"//7
};

enum Errors {
    DATA_NULLPTR,//0
    INVALID_SIZE,//1
    INVALID_CAPACITY,//2
    SIZE_LARGER_TH_CAPACITY,//3
    STACK_CANARY_L_ERR,//4
    STACK_CANARY_R_ERR,//5
    DATA_CANARY_L_ERR,//6
    DATA_CANARY_R_ERR,//7
    AMOUNT_OF_POSSIBLE_ERRORS /*must be last*/
};

const ULL STACK_CANARY_L = 0xBAADBEEF;
const ULL STACK_CANARY_R = 0xBAADBABE;
const ULL DATA_CANARY_L = 0xDEADBEEF;
const ULL DATA_CANARY_R = 0xDEADBABE;

const ULL DEFAULT_HASH = 69300;


const size_t default_size = 5;

int main() {
    struct Stack stk1;
    printf("Elem t is %d, ULL is %d\n", sizeof(Elem_t), sizeof(ULL));


    StackCtor(stk1, default_size);

    printf("1 right data canary is %x\n", *stk1.data_canary_r);
    printf("1 left data canary is %x\n", *stk1.data_canary_l);

    StackPush(&stk1, 1);
    StackPush(&stk1, 2);
    StackPush(&stk1, 3);
    StackPush(&stk1, 4);
    StackPush(&stk1, 5);
    printf("2 right data canary is %x\n", *stk1.data_canary_r);
    printf("2 left data canary is %x\n", *stk1.data_canary_l);
    StackPush(&stk1, 6);
    printf("3 right data canary is %x\n", *stk1.data_canary_r);
    printf("3 left data canary is %x\n", *stk1.data_canary_l);
    printf("1st stackpop is %d\n", StackPop(&stk1));
    printf("2nd stackpop is %d\n", StackPop(&stk1));
    printf("3rd stackpop is %d\n", StackPop(&stk1));
    printf("4th stackpop is %d\n", StackPop(&stk1));
    printf("5th stackpop is %d\n", StackPop(&stk1));
    printf("6th stackpop is %d\n", StackPop(&stk1));
    printf("7th stackpop is %d\n", StackPop(&stk1));
    printf("8th stackpop is %d\n", StackPop(&stk1));
    printf("9th stackpop is %d\n", StackPop(&stk1));
    printf("10th stackpop is %d\n", StackPop(&stk1));

    printf("\n");

    for (int i = 0; i < stk1.capacity+1; i++) {
        printf("%x ", stk1.data[i]);
    }

return 0;
}

void StackDtor(struct Stack *stk){
    free(stk->data);
}

Elem_t StackPop(struct Stack *stk) {
    ASSERT_OK(stk);

    if (stk->ssize <= stk->capacity/4) {
        printf("actually need to shrink\n");
        StackRecalloc(stk, stk->capacity/2);
        printf("shrink in pop done\n");
    }

    Elem_t tmp = stk->data[--stk->ssize];
    stk->data[stk->ssize] = getPoison(stk->data[stk->ssize]);

    ASSERT_OK(stk);
    return tmp;
}

void StackPush(struct Stack *stk, Elem_t value) {
    ASSERT_OK(stk);

    if (stk->ssize == stk->capacity) {
        StackRecalloc(stk, stk->capacity*2);
        printf("stackRecalloc done\n");
    }
    stk->data[stk->ssize++] = value;

    if (checkHash(stk)) {
        printf("on line %d\n", __LINE__);
    }

    ASSERT_OK(stk);
}

void StackRecalloc(struct Stack *stk, size_t ssize){
   // printf("before recalloc data is %p\n", stk->data);
    char* tmp = (char*)stk->data;
   // printf("tmp now %p\n", tmp);
    tmp -= sizeof(ULL);
    //printf("after moving to ull tmp is %p\n", tmp);

    tmp = (char*)(realloc((char*)tmp, ssize*sizeof(Elem_t) + 2*sizeof(ULL)));

   // printf("second canary place is %d\n", ssize*sizeof(Elem_t) + sizeof(ULL));

   // printf("after realloc tmp is %p\n", tmp);

    //printf("old data_canary_l is %p\n", stk->data_canary_l);
    stk->data_canary_l = (ULL*) tmp;
   // printf("new data_canary_l is %p\n", stk->data_canary_l);

   // printf("old data_canary_r is %p\n", stk->data_canary_r);
    stk->data_canary_r = (ULL*) (tmp + sizeof(ULL) + ssize * sizeof(Elem_t));
    //printf("new data_canary_r is %p\n", stk->data_canary_r);

    *(stk->data_canary_l)= DATA_CANARY_L;
    *(stk->data_canary_r) = DATA_CANARY_R;

    stk->data = (Elem_t*)(tmp + sizeof(ULL));
    //printf("final data is %p\n", stk->data);

    if (ssize > stk->capacity) {
        for (size_t i = 0; i < ssize - stk->capacity; i++) {
            stk->data[stk->ssize + i] = getPoison(stk->data[stk->ssize + i]);
        }
    }
    //printf("passed it\n");
    stk->capacity = ssize;
}

void printStack(int *data, size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] != getPoison(data[i])){
            logprint("    *[%zd] = %d\n", i, data[i]);
        } else {
            logprint("    [%zd] = %x (POISON)\n", i, data[i]);
        }
    }
}

void printStack(double *data, size_t capacity){
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] != getPoison(data[i])){
            logprint("    *[%zd] = %f\n", i, data[i]);
        } else {
            logprint("    [%zd] = %x (POISON)\n", i, data[i]);
        }
    }
}

void printStack(char *data, size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] != getPoison(data[i])){
            logprint("    *[%zd] = %c\n", i, data[i]);
        } else {
            logprint("    [%zd] = %x (POISON)\n", i, data[i]);
        }
    }


}

unsigned long StackError(struct Stack *stk) {

    if (stk->data == NULL)
            stk->errors |= (1 << DATA_NULLPTR);

    if (stk->ssize > stk->capacity)
            stk->errors |= (1 << SIZE_LARGER_TH_CAPACITY);

    if ((int)stk->ssize < 0)
            stk->errors |= (1 << INVALID_SIZE);

    if ((int)stk->capacity < 0)
            stk->errors |= (1 << INVALID_CAPACITY);

    if (stk->stack_canary_l != STACK_CANARY_L)
            stk->errors |= (1 << STACK_CANARY_L_ERR);

    if (stk->stack_canary_r != STACK_CANARY_R)
            stk->errors |= (1 << STACK_CANARY_R_ERR);

    if (*(stk->data_canary_l) != DATA_CANARY_L)
            stk->errors |= (1 << DATA_CANARY_L_ERR);

    if (*(stk->data_canary_r) != DATA_CANARY_R)
            stk->errors |= (1 << DATA_CANARY_R_ERR);

    return(stk->errors);
}

void printErr(unsigned long error) {
    for (size_t i = 0; i < AMOUNT_OF_POSSIBLE_ERRORS; i++) {
        if (error & (1 << i))
            printf("ERROR: %s\n", ErrorNames[i]);

    }
}

int getPoison(int) {
    return 0xCAFEBABE;
}

size_t getPoison(size_t) {
    return 0xCAFEBABE;
}

double getPoison(double) {
    return NAN;
}

char getPoison(char) {
    return 'z' + 1;
}

char* getPoison(char*) {
    return (char*)0xCAFEBABE;
}

ULL calcGNUhash(void* start_ptr, int num_bytes) {
    ULL hash_sum = DEFAULT_HASH;
    for (size_t i = 0; i < num_bytes; i++) {
        hash_sum = hash_sum * 33 + ((char*)start_ptr)[i];
    }
    return hash_sum;
}

int checkHash(struct Stack *stk) {
    int status = 0;

    ULL tmp_data_hash =  stk->data_hash_sum; /*was updated by previous ASSERT_OK*/
    stk->data_hash_sum = 0;
    ULL n_data_hash_sum = calcGNUhash(stk->data, stk->capacity * sizeof(Elem_t));
    if (tmp_data_hash != n_data_hash_sum) {
        printf("\n\nCAREFUL, DATA_HASH_SUM WAS MODIFIED ");
        stk->data_hash_sum = n_data_hash_sum;
        status += 1;
    }

    ULL tmp_stack_hash =  stk->stack_hash_sum; /*was updated by previous ASSERT_OK*/
    stk->stack_hash_sum = 0;
    ULL n_stack_hash_sum = calcGNUhash(stk, sizeof(*stk));
    if (tmp_stack_hash != n_stack_hash_sum) {
        printf(" STACK_HASH_SUM WAS MODIFIED ");
        stk->stack_hash_sum = n_stack_hash_sum;
        status += 1;
    }
    if (status == 0) {
        printf("data_hash_sum and stack_hash_sum were not modified\n");
    }

    return (status?1:0);

}








