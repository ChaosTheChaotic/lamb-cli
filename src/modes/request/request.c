#include "string.h"
#include "subcmd.h"
#include "keys.h"
#include "liblambproto.h"
#include "libcroco.h"
#include <stdio.h>
#include <stdlib.h>

int request(unsigned int argc, Arg *args) {
  char *path = NULL;
  char *restrict pwd = NULL;
  bool cliflag = false;
  bool np = false;

  for (unsigned int i = 0; i < argc; i++) {
    if (args[i].flag == 'i' && args[i].found) {
      cliflag = true;
      path = args[i].value.str_val;
    } else if (args[i].flag == 'p' && args[i].found) {
      np = true;
    }
  }

  if (path == NULL || path[0] == '\0') {
    cliflag = true;
    path = NULL;
    size_t size = 0;
    printf("Enter the path you want to request (must be accessible via adb): ");
    ssize_t getle = getline(&path, &size, stdin);
    if (getle == -1) {
      fprintf(stderr, "Failed to get user input using getline, specify a file path using -i");
      free(path);
      return -1;
    }
    if (getle > 0 && path[getle - 1] == '\n') {
      path[getle - 1] = '\0';
    }
  }

  if (!np) pwd = get_pwd();
  if (!pwd) {
    prompt_pwd("Enter password (must be same as android): ", &pwd);
  }

  CrocoHeader header;
  CrocoHeader_init(&header);

  header.protocolName = strdup("request");
  header.extraOptFlags = malloc(sizeof(KeyValuePair));
  header.extraOptFlags[0].key = strdup("path");
  header.extraOptFlags[0].value = strdup(path);
  header.extraOptFlags_count = 1;

  char *shead = CrocoHeader_serialize(&header);
  if (!shead) {
    fprintf(stderr, "Failed to seralize protocol header");
    CrocoHeader_free(&header);
    if (cliflag) free(path);
    free(pwd);
    return -1;
  }

  char *res = SendText(shead, pwd);
  printf("%s", res);

  free(shead);
  CrocoHeader_free(&header);
  if (cliflag) free(path);
  free(pwd);
  return 0;
}
