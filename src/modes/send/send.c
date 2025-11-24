#include "libcrocou.h"
#include <stdio.h>
#include "subcmd.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int send(unsigned int argc, Arg *args) {
  char *msg = NULL;
  char *pwd = NULL;
  bool cliflag = false;

  for (unsigned int i = 0; i < argc; i++) {
    if (args[i].flag == 'm' && args[i].found) {
      cliflag = true;
      msg = args[i].value.str_val;
      break;
    }
  }

  // If we dont have a message prompt user for one
  if (msg == NULL || strcmp(msg, "Hello, world!") == 0) {
    cliflag = true;
    msg = NULL; // Set msg to null so getline mallocs
    size_t size = 0;
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

  // Get password

  if (cliflag) {
    free(msg);
  }
  return 0;
}
