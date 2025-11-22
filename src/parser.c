#include <string.h>
#include <stdio.h>
#include "parser.h"

/**
 * Parses the subcommand type from command line arguments
 * @param argc Argument count
 * @param argv Argument list
 * @param result Output parameter for found subcommand
 * @return true if valid subcommand found, false otherwise
 */
bool parse_type(int argc, char** argv, SubCommand* result) {
    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < subcommand_count; j++) {
            if (strcasecmp(argv[i], valid_subcommands[j].name) == 0) {
                *result = valid_subcommands[j];
                return true;
            }
        }
    }
    return false;
}

/**
 * Parses arguments for a given subcommand
 * @param argc Argument count
 * @param argv Argument list
 * @param subcmd Subcommand to parse arguments for
 * @return true if arguments valid, false otherwise
 */
bool parse_args(int argc, char** argv, SubCommand* subcmd) {
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] == '-' && strlen(argv[i]) == 2) {
            char flag = argv[i][1];
            bool found = false;
            
            for (int j = 0; j < subcmd->arg_count; j++) {
                if (subcmd->args[j].flag == flag) {
                    subcmd->args[j].found = true;
                    found = true;
                    
                    if (subcmd->args[j].type == BOOL) {
                        subcmd->args[j].value.bool_val = true;
                    } else if (i + 1 < argc) {
                        char* next_arg = argv[++i];
                        switch (subcmd->args[j].type) {
                            case INT:
                                if (sscanf(next_arg, "%d", &subcmd->args[j].value.int_val) != 1)
                                    return false;
                                break;
                            case FLOAT:
                                if (sscanf(next_arg, "%f", &subcmd->args[j].value.float_val) != 1)
                                    return false;
                                break;
                            case STRING:
                                subcmd->args[j].value.str_val = next_arg;
                                break;
                            default:
                                break;
                        }
                    } else {
                        return false; // Missing value for non-bool flag
                    }
                    break;
                }
            }
            if (!found) {
                printf("Unknown flag: -%c\n", flag);
                return false;
            }
        } else {
            printf("Invalid argument format: %s\n", argv[i]);
            return false;
        }
    }

    // Validate required arguments
    for (int i = 0; i < subcmd->arg_count; i++) {
        if (subcmd->args[i].required && !subcmd->args[i].found) {
            printf("Missing required flag: -%c\n", subcmd->args[i].flag);
            return false;
        }
    }

    return true;
}
