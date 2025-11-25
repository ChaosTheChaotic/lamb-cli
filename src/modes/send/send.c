#include "libcrocou.h"
#include "liblambproto.h"
#include "subcmd.h"
#include "keys.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int send(unsigned int argc, Arg *args) {
    char *msg = NULL;
    char *restrict pwd = NULL;
    bool cliflag = false;
    bool np = false;

    for (unsigned int i = 0; i < argc; i++) {
        if (args[i].flag == 'm' && args[i].found) {
            cliflag = true;
            msg = args[i].value.str_val;
            break;
        } else if (args[i].flag == 'p' && args[i].found) {
            np = true;
        }
    }

    // Get message if not given
    if (msg == NULL || msg[0] == '\0') {
        cliflag = true;
        msg = NULL;
        size_t size = 0;
        printf("Enter the message you want to send: ");
        ssize_t getle = getline(&msg, &size, stdin);
        if (getle == -1) {
            fprintf(stderr, "Failed to get user input using getline, try to specify a message using the -m flag");
            free(msg);
            return -1;
        }
        if (getle > 0 && msg[getle - 1] == '\n') {
            msg[getle - 1] = '\0';
        }
    }

    if (!np) pwd = get_pwd();
    if (!pwd) {
        prompt_pwd("Enter password (must be same as android): ", &pwd);
    }

    CrocoHeader header = {
        .protocolName = "send",
        .extraOptFlags = (KeyValuePair[]){
            {.key = "message", .value = msg},
            // {.key = "another_key", .value = "another_value"}
        },
        .extraOptFlags_count = 1
    };

    char *serialized_header = CrocoHeader_serialize(&header);
    if (!serialized_header) {
        fprintf(stderr, "Failed to serialize protocol header");
        if (cliflag) free(msg);
        free(pwd);
        return -1;
    }

    char *res = SendText(serialized_header, pwd);
    printf("%s", res);

    free(serialized_header);
    if (cliflag) free(msg);
    free(pwd);
    return 0;
}
