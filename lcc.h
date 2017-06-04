#ifndef LCC_H
#define LCC_H

#include "ADT.h"

#define TOP_STACK get_top_scope(symtab)

#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


typedef enum Type_size {
    BYTE,
    WORD,
    LONG_WORD,
    QUAD_WORD
} Type_size;

typedef enum Type {
    DCHAR = BYTE,
    DSHORT = WORD,
    DINT = LONG_WORD,
    DLONG = QUAD_WORD,
    DFUNC,          // function definition: 4
    FUNC_DECL,     // function declaration: 5
    FUNC_CALL,
    NEW_SCOPE,      // new scope, e.g. function, for, while, if
    NOT_KNOWN,
} Type;

static int real_size[] = {
        1,
        2,
        4,
        8,
};

static char op_suffix[] = {
        'b', 'w', 'l', 'q',
};

static char *regular_reg[][4] = {
        {
                "al",
                "ax",
                "eax",
                "rax",
        },
        {
                "bl",
                "bx",
                "ebx",
                "rbx",
        },
        {
                "cl",
                "cx",
                "ecx",
                "rcx",

        }
};

static char *arguments_reg[][4] = {
        {
                "dil",
                "di",
                "edi",
                "rdi",
        },
        {

                "sil",
                "si",
                "esi",
                "rsi",
        },
        {
                "dl",
                "dx",
                "edx",
                "rdx",
        },
        {
                "cl",
                "cx",
                "ecx",
                "rcx",

        }
};

extern FILE *output;

typedef struct Assembly {
    List_node *beg, *end;
} Assembly;

Assembly *make_assembly();

void assembly_push_back(Assembly *ptr, String *code);

void assembly_append(Assembly *ptr, Assembly *p);

void assembly_push_front(Assembly *ptr, String *code);

void assembly_output(Assembly *ptr);

#define INVALID_OFFSET 0xFFFFFF

typedef struct Stack {
    // normal offset in stack
    int offset;

    // array-related
    Vector *step;

    // top of whole stack
    int rsp;
} Stack;

int allocate_stack(int);

int allocate_stack(int);

void free_stack(int);


typedef struct Value {
    // 1: tmp var on stack 2: constant
    int index;

    // for constant
    int int_num;

    // for tmp var
    int offset;
    Type_size size;

    // for array
    Vector *step;
    int cur_dimension;
} Value;

Value *make_constant_val(int val);

Value *make_stack_val(int offset, Type_size size);

Value *clone_value(Value *);

int has_constant(Value *);

void set_constant(Value *, int val);

int get_constant(Value *);

int has_stack_offset(Value *);

void set_value_info(Value *, int offset, Type_size size);

Type_size get_type_size(Value *);

int get_stack_offset(Value *);

typedef struct Label {
    int beg_label;
    int end_label;
    int exit_label;
} Label;

void set_control_label(Label *);

void set_exit_label(Label *);

String *get_beg_label(Label *);

String *get_end_label(Label *);

String *get_exit_label(Label *);

typedef struct Analysis {
    String *name;
    Type self_type;

    // for code-generated
    Assembly *assembly;

    // for scope
    struct Analysis *parent;

    // for function
    Type ret_type;
    Vector *param;
    int rsp;

    // varaible offset in stack
    int offset;

    // array-related step
    Vector *step;

    // for var usage
    Value res_info;
} Analysis;

typedef Analysis Symbol;

void add_dimension(Symbol *s, int d);

void convert_dimension_to_step();

void yyerror(const char *fmt, ...);

void free_variables(Symbol *);

void signal_extend(Assembly *code, int idx, Type_size original, Type_size new);

void pop_and_je(Assembly *code, Value *op1, String *if_equal);

Value *pop_and_op(Assembly *code, Value *op1, char *op_prefix, Value *op2);

int pop_and_index(Assembly *code, Value *op1, char *op_prefix, Value *op2, Stack *);

Value *pop_and_single_op(Assembly *code, Value *op1, char *op_prefix, Value *op2);

Value *pop_and_shift(Assembly *code, Value *op1, char *op_prefix, Value *op2);

Value *pop_and_set(Assembly *code, Value *op1, char *op_prefix, Value *op2);

void emit_jump(Assembly *code, String *label);

void emit_push_var(Assembly *code, Value *res_info);

int emit_push_register(Assembly *code, size_t idx, Type_size size);

void emit_pop(Assembly *code, Value *res_info, size_t idx);

void emit_func_signature(Assembly *code, String *str);

void emit_get_func_arguments(Assembly *code);

void emit_set_func_arguments(Assembly *code, Symbol *func);

void emit_local_variable(Assembly *code);

void add_while_label(Symbol *cond, Analysis *stat);

Symbol *make_symbol();

Symbol *symbol_cast(void *);

void enter_func_def_symbol(Type ret_type, String *name, Vector *param);

void exit_func_def();

void make_func_decl_symbol(Type ret_type, String *name, Vector *param);

void make_local_symbol(Type, String *name, Vector *step, Value res_info);

Symbol *make_param_symbol(Type, String *name);

void make_new_scope();

void destroy_new_scope();

Symbol *find_name(String *name);

Symbol *get_top_scope();

int in_global_scope();

int is_array();

#define YYSTYPE Analysis

#include "y.tab.h"

#endif