#include "liblambproto.h"
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
#define FIELD_KV_PAIRS(name, _)                                                \
  if (header->name && header->name##_count > 0) {                              \
    for (int i = 0; i < header->name##_count; i++) {                           \
      if (header->name[i].key)                                                 \
        total_size += strlen(header->name[i].key) + 1; /* +1 for '=' */        \
      if (header->name[i].value)                                               \
        total_size += strlen(header->name[i].value);                           \
      if (i < header->name##_count - 1)                                        \
        total_size += 1; /* for ',' */                                         \
    }                                                                          \
    total_size += 1; /* for '|' */                                             \
  } else {                                                                     \
    total_size += 1; /* for empty field '|' */                                 \
  }
  PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_KV_PAIRS

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
#define FIELD_KV_PAIRS(name, _)                                                \
  if (header->name && header->name##_count > 0) {                              \
    for (int i = 0; i < header->name##_count; i++) {                           \
      if (header->name[i].key) {                                               \
        size_t len = strlen(header->name[i].key);                              \
        memcpy(cursor, header->name[i].key, len);                              \
        cursor += len;                                                         \
      }                                                                        \
      *cursor++ = '=';                                                         \
      if (header->name[i].value) {                                             \
        size_t len = strlen(header->name[i].value);                            \
        memcpy(cursor, header->name[i].value, len);                            \
        cursor += len;                                                         \
      }                                                                        \
      if (i < header->name##_count - 1)                                        \
        *cursor++ = ',';                                                       \
    }                                                                          \
  }                                                                            \
  *cursor++ = '|';
  PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_KV_PAIRS

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
        // Count key-value pairs by counting commas
        size_t count = 1;
        for (size_t i = 0; i < field_len; i++) {
          if (cursor[i] == ',')
            count++;
        }

        header->extraOptFlags = malloc(count * sizeof(KeyValuePair));
        header->extraOptFlags_count = 0;

        char *temp = malloc(field_len + 1);
        memcpy(temp, cursor, field_len);
        temp[field_len] = '\0';

        char *token = strtok(temp, ",");
        while (token && (size_t)header->extraOptFlags_count < count) {
          char *equal_sign = strchr(token, '=');
          if (equal_sign) {
            // Split key and value
            *equal_sign = '\0';
            header->extraOptFlags[header->extraOptFlags_count].key =
                strdup(token);
            header->extraOptFlags[header->extraOptFlags_count].value =
                strdup(equal_sign + 1);
          } else {
            // No value, just key
            header->extraOptFlags[header->extraOptFlags_count].key =
                strdup(token);
            header->extraOptFlags[header->extraOptFlags_count].value = NULL;
          }
          header->extraOptFlags_count++;
          token = strtok(NULL, ",");
        }
        free(temp);
      } else {
        header->extraOptFlags = NULL;
        header->extraOptFlags_count = 0;
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

// Helper function to free the header
void CrocoHeader_free(CrocoHeader *header) {
  if (!header)
    return;

#define FIELD_STRING(name, _)                                                  \
  if (header->name)                                                            \
    free(header->name);
#define FIELD_KV_PAIRS(name, _)                                                \
  if (header->name) {                                                          \
    for (int i = 0; i < header->name##_count; i++) {                           \
      if (header->name[i].key)                                                 \
        free(header->name[i].key);                                             \
      if (header->name[i].value)                                               \
        free(header->name[i].value);                                           \
    }                                                                          \
    free(header->name);                                                        \
    header->name##_count = 0;                                                  \
  }
  PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_KV_PAIRS
}
