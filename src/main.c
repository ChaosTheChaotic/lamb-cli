#include "parser.h"
#include "send.h"
#include "request.h"
#include "subcmd.h"
#include <stdio.h>
#include <string.h>

void printHelp(char *path) {
  printf("Usage: %s <subcommand> [flags]\n", path);
  printf("subcommands:\n");
  printf("send: sends a string to appear as a notification on the linked android\n");
  printf("send has custom args:\n");
  printf("  -m sends a custom message\n");
  printf("  -p allows specifying a new password\n");
  printf("request: requests a file from the android\n");
  printf("request has custom args:\n");
  printf("  -i for a custom input file\n");
  printf("  -p allows specifying a new password\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printHelp(argv[0]);
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
  if (strcmp(selected_cmd.name, "send") == 0) {
    send(selected_cmd.arg_count, selected_cmd.args);
  } else if (strcmp(selected_cmd.name, "request") == 0) {
    request(selected_cmd.arg_count, selected_cmd.args);
  } else {
    printHelp(argv[0]);
  }
  return 0;
}
