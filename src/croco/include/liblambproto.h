#ifndef LIBLAMBP_H
#define LIBLAMBP_H

#include <stdbool.h>

typedef struct {
  // Requiring a response or not can be implied from the name and flags
  char *protocolName;
  char **extraOptFlags;
}CrocoHeader;

#endif
