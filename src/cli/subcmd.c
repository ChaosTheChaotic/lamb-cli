#include "subcmd.h"

Arg send_args[] = {
    {'a', BOOL, false, {.bool_val = false}, false}, // TODO: Implement async
    {'m', STRING, false, {.str_val = ""}, false}, // Message flag
    {'p', STRING, false, {.str_val = ""}, false}, // Password flag (specify a new password to save)
};
const int send_arg_count = sizeof(send_args) / sizeof(Arg);

Arg request_args[] = {
    {'a', BOOL, false, {.bool_val = false}, false},
};
const int request_arg_count = sizeof(request_args) / sizeof(Arg);

const SubCommand valid_subcommands[] = {
    {"send", send_args, send_arg_count},
    {"request", request_args, request_arg_count},
};

const int subcommand_count = sizeof(valid_subcommands) / sizeof(SubCommand);
