#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED


#ifndef NDEBUG
    #define ASSERT_OK(stk)                                                    \
        stk->data_hash_sum = 0;                                                      \
        stk->data_hash_sum = calcGNUhash(stk->data, stk->capacity * sizeof(Elem_t)); \
        stk->stack_hash_sum = 0;                                                    \
        stk->stack_hash_sum = calcGNUhash(stk, sizeof(*stk));                 \
        StackDump(stk)                                                        \
        if (StackError(stk))                                                  \
        {                                                                     \
            printErr(stk->errors);                                            \
            exit(0);                                                          \
        }


#else
    #define ASSERT_OK(stk) ;
#endif


#define StackDump(stk)                                                      \
    logprint("%s at %s(%d)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);    \
    if (stk == NULL)                                                        \
        logprint("invalid stack pointer, can't dump\n");                    \
    logprint("Stack[%p] \"%s\" at %s at %s(%d)\n", stk, stk->stk_disc.name, \
            stk->stk_disc.func, stk->stk_disc.file, stk->stk_disc.line);    \
    logprint("{\n");                                                        \
    logprint("  size = %zd\n", stk->ssize);                                 \
    logprint("  capacity = %zd\n", stk->capacity);                          \
    logprint("  data[%p]\n", stk->data);                                    \
    if (stk->data == NULL) {                                                \
        logprint("invalid data pointer, cant't print stack\n");             \
        logprint("}\n");                                                    \
    } else {                                                                \
        logprint("  {\n");                                                      \
        printStack(stk->data, stk->capacity);                                   \
        logprint("  }\n");                                                      \
        logprint("}\n");                                                        \
    }

#define SetDescription(stk, stk_desc)                                      \
    stk_desc.line = __LINE__;                                              \
    stk_desc.name = #stk;                                                  \
    stk_desc.func = (char*)__PRETTY_FUNCTION__;                            \
    stk_desc.file = (char*)__FILE__;


#define StackCtor(stk, scale)                                              \
    stk.stack_canary_l = STACK_CANARY_L;                                   \
    stk.stack_canary_r = STACK_CANARY_R;                                   \
    SetDescription(stk, stk.stk_disc);                                     \
    stk.capacity = scale;                                                  \
    stk.errors = 0;                                                        \
    stk.ssize = 0;                                                         \
    size_t enlarged_size = scale * sizeof(Elem_t) + 2 * sizeof(ULL);       \
    char* tmp  = (char*)(calloc(enlarged_size, sizeof(char)));              \
    stk.data_canary_l = (ULL*)tmp;                                          \
    stk.data_canary_r = (ULL*)(tmp + sizeof(ULL) + scale*sizeof(Elem_t));   \
    stk.data = (Elem_t*)(tmp + sizeof(ULL));                                  \
    *(stk.data_canary_l)= DATA_CANARY_L;                                     \
    *(stk.data_canary_r) = DATA_CANARY_R;                                     \
    printf("calloc done with pointer %p\n", stk.data);






typedef int Elem_t;
typedef unsigned long long ULL;


#endif // STACK_H_INCLUDED
