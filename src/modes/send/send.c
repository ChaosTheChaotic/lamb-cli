#include "libcrocou.h"
#include "subcmd.h"
#include "pwds.h"
#include <string.h>

int send(unsigned int argc, Arg *args) {
  char *msg = NULL;
  char *pwd = NULL;

  for (unsigned int i = 0; i < argc; i++) {
    if (args[i].flag == 'm' && args[i].found) {
      msg = args[i].value.str_val;
      break;
    }
  }

  if (msg == NULL || strcmp(msg, "Hello, world!") == 0) {
    // Prompt user for message input here
  }
}
