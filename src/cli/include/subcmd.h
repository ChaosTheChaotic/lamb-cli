#ifndef SUBCMD_H
#define SUBCMD_H

#include <stdbool.h>

typedef enum {
    INT,
    FLOAT,
    STRING,
    BOOL
} ArgType;

typedef struct {
    char flag;
    ArgType type;
    bool required;
    union {
        int int_val;
        float float_val;
        char* str_val;
        bool bool_val;
    } value;
    bool found;
} Arg;

typedef struct {
    char* name;
    Arg* args;
    int arg_count;
} SubCommand;

extern const SubCommand valid_subcommands[];
extern const int subcommand_count;

#endif
