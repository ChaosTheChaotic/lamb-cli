#include "liblambproto.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Serialize header to string
char *CrocoHeader_serialize(const CrocoHeader *header) {
  if (!header)
    return NULL;

  // Calculate required buffer size
  size_t total_size = 1; // Null terminator

#define FIELD_STRING(name, _)                                                  \
  if (header->name)                                                            \
    total_size += strlen(header->name) + 1;
#define FIELD_STRING_ARRAY(name, _)                                            \
  if (header->name) {                                                          \
    for (char **ptr = header->name; *ptr; ptr++) {                             \
      total_size += strlen(*ptr) + 1;                                          \
    }                                                                          \
  }
  PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_STRING_ARRAY

  char *result = malloc(total_size);
  if (!result)
    return NULL;

  char *cursor = result;

  // Serialize fields
#define FIELD_STRING(name, _)                                                  \
  if (header->name) {                                                          \
    size_t len = strlen(header->name);                                         \
    memcpy(cursor, header->name, len);                                         \
    cursor += len;                                                             \
  }                                                                            \
  *cursor++ = '|';
#define FIELD_STRING_ARRAY(name, _)                                            \
  if (header->name) {                                                          \
    for (char **ptr = header->name; *ptr; ptr++) {                             \
      size_t len = strlen(*ptr);                                               \
      memcpy(cursor, *ptr, len);                                               \
      cursor += len;                                                           \
      if (*(ptr + 1))                                                          \
        *cursor++ = ',';                                                       \
    }                                                                          \
  }                                                                            \
  *cursor++ = '|';
  PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_STRING_ARRAY

  cursor[-1] = '\0'; // Replace last '|' with null terminator
  return result;
}

// Deserialize string to header
bool CrocoHeader_deserialize(CrocoHeader *header, const char *data) {
  if (!header || !data)
    return false;

  const char *cursor = data;
  int field_index = 0;

  while (*cursor && field_index < 2) { // Update count when adding fields
    const char *field_end = strchr(cursor, '|');
    if (!field_end)
      field_end = cursor + strlen(cursor);

    size_t field_len = field_end - cursor;

    switch (field_index) {
    case 0: // protocolName
      if (field_len > 0) {
        header->protocolName = malloc(field_len + 1);
        memcpy(header->protocolName, cursor, field_len);
        header->protocolName[field_len] = '\0';
      } else {
        header->protocolName = NULL;
      }
      break;

    case 1: // extraOptFlags
      if (field_len > 0) {
        // Count commas to determine array size
        size_t count = 1;
        for (size_t i = 0; i < field_len; i++) {
          if (cursor[i] == ',')
            count++;
        }

        header->extraOptFlags = malloc((count + 1) * sizeof(char *));
        char *temp = malloc(field_len + 1);
        memcpy(temp, cursor, field_len);
        temp[field_len] = '\0';

        char *token = strtok(temp, ",");
        size_t idx = 0;
        while (token) {
          header->extraOptFlags[idx] = strdup(token);
          token = strtok(NULL, ",");
          idx++;
        }
        header->extraOptFlags[idx] = NULL;
        free(temp);
      } else {
        header->extraOptFlags = NULL;
      }
      break;
    }

    cursor = field_end;
    if (*cursor == '|')
      cursor++;
    field_index++;
  }

  return true;
}
