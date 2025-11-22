#include <stdio.h>
#include "include/subcmd.h"
#include "include/parser.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <subcommand> [flags]\n", argv[0]);
        return 1;
    }

    SubCommand selected_cmd;
    if (!parse_type(argc, argv, &selected_cmd)) {
        printf("Invalid subcommand. Valid commands: ");
        for (int i = 0; i < subcommand_count; i++) {
            printf("%s ", valid_subcommands[i].name);
        }
        printf("\n");
        return 1;
    }

    if (!parse_args(argc, argv, &selected_cmd)) {
        printf("Invalid arguments for subcommand '%s'\n", selected_cmd.name);
        return 1;
    }

    printf("Using mode %s\n", selected_cmd.name);
    for (int i = 0; i < selected_cmd.arg_count; i++) {
        if (selected_cmd.args[i].found) {
            printf("Flag -%c: ", selected_cmd.args[i].flag);
            switch (selected_cmd.args[i].type) {
                case BOOL:
                    printf("%s\n", selected_cmd.args[i].value.bool_val ? "true" : "false");
                    break;
                case INT:
                    printf("%d\n", selected_cmd.args[i].value.int_val);
                    break;
                case FLOAT:
                    printf("%f\n", selected_cmd.args[i].value.float_val);
                    break;
                case STRING:
                    printf("%s\n", selected_cmd.args[i].value.str_val);
                    break;
            }
        }
    }

    return 0;
}
