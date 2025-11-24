#include "libcrocou.h"
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

  for (unsigned int i = 0; i < argc; i++) {
    if (args[i].flag == 'm' && args[i].found) {
      cliflag = true;
      msg = args[i].value.str_val;
      break;
    }
  }

  // If we dont have a message prompt user for one
  if (msg == NULL || msg[0] == '\0') {
    cliflag = true;
    msg = NULL; // Set msg to null so getline mallocs
    size_t size = 0;
    printf("Enter the message you want to send: ");
    ssize_t getle = getline(&msg, &size, stdin);
    if (getle == -1) {
      fprintf(stderr, "Failed to get user input using getline, try to specify a message using the -m flag");
      free((char *)msg);
      return -1;
    }
    if (getle > 0 && msg[getle - 1] == '\n') { // The buffer is null terminated and also could include a newline
      msg[getle - 1] = '\0'; // Null terminate
    }
  }

  pwd = get_pwd();
  if (!pwd) {
    prompt_pwd("Enter password (must be same as android): ", pwd);
  }

  char *res = SendText(msg, pwd);
  printf("%s", res);

  if (cliflag) free(msg);
  free(pwd);
  return 0;
}
