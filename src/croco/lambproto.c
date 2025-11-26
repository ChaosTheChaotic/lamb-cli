#include "liblambproto.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize header with default values
void CrocoHeader_init(CrocoHeader *header) {
  if (!header)
    return;

#define FIELD_STRING(name, _) header->name = NULL;
#define FIELD_KV_PAIRS(name, _)                                                \
  header->name = NULL;                                                         \
  header->name##_count = 0;
  PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_KV_PAIRS
}

bool parse_key_value_pairs(const char *data, size_t len, KeyValuePair **pairs,
                           int *count) {
  if (len == 0) {
    *pairs = NULL;
    *count = 0;
    return true;
  }

  // Count key-value pairs
  size_t pair_count = 1;
  for (size_t i = 0; i < len; i++) {
    if (data[i] == ',')
      pair_count++;
  }

  KeyValuePair *result = malloc(pair_count * sizeof(KeyValuePair));
  if (!result)
    return false;

  char *temp = malloc(len + 1);
  if (!temp) {
    free(result);
    return false;
  }
  memcpy(temp, data, len);
  temp[len] = '\0';

  char *token = strtok(temp, ",");
  size_t index = 0;
  while (token && index < pair_count) {
    char *equal_sign = strchr(token, '=');
    if (equal_sign) {
      *equal_sign = '\0';
      result[index].key = strdup(token);
      result[index].value = strdup(equal_sign + 1);
    } else {
      result[index].key = strdup(token);
      result[index].value = NULL;
    }

    if (!result[index].key) {
      // Cleanup on failure
      for (size_t i = 0; i < index; i++) {
        free(result[i].key);
        free(result[i].value);
      }
      free(result);
      free(temp);
      return false;
    }

    index++;
    token = strtok(NULL, ",");
  }

  *pairs = result;
  *count = index;
  free(temp);
  return true;
}

// Serialize header to string
char *CrocoHeader_serialize(const CrocoHeader *header) {
  if (!header)
    return NULL;

  // Calculate required buffer size
  size_t total_size = 1; // Null terminator

#define FIELD_STRING(name, _)                                                  \
  if (header->name)                                                            \
    total_size += strlen(header->name);                                        \
  total_size += 1; /* for '|' */
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
  }                                                                            \
  total_size += 1; /* for '|' */
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

  // Initialize header to clean state
  CrocoHeader_init(header);

  const char *cursor = data;
  int field_index = 0;

  while (*cursor) {
    const char *field_end = strchr(cursor, '|');
    if (!field_end)
      field_end = cursor + strlen(cursor);

    size_t field_len = field_len = field_end - cursor;

    // Use the macro to handle field deserialization
    bool field_processed = false;

#define FIELD_STRING(name, idx)                                                \
  if (field_index == idx) {                                                    \
    if (field_len > 0) {                                                       \
      header->name = malloc(field_len + 1);                                    \
      memcpy(header->name, cursor, field_len);                                 \
      header->name[field_len] = '\0';                                          \
    }                                                                          \
    field_processed = true;                                                    \
  }
#define FIELD_KV_PAIRS(name, idx)                                              \
  if (field_index == idx) {                                                    \
    if (!parse_key_value_pairs(cursor, field_len, &header->name,               \
                               &header->name##_count)) {                       \
      CrocoHeader_free(header);                                                \
      return false;                                                            \
    }                                                                          \
    field_processed = true;                                                    \
  }
    PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_KV_PAIRS

    if (!field_processed) {
      // Unknown field, skip it
      break;
    }

    cursor = field_end;
    if (*cursor == '|')
      cursor++;
    field_index++;
  }

  return true;
}

void CrocoHeader_free(CrocoHeader *header) {
  if (!header)
    return;

#define FIELD_STRING(name, _)                                                  \
  if (header->name) {                                                          \
    free(header->name);                                                        \
    header->name = NULL;                                                       \
  }
#define FIELD_KV_PAIRS(name, _)                                                \
  if (header->name) {                                                          \
    for (int i = 0; i < header->name##_count; i++) {                           \
      if (header->name[i].key)                                                 \
        free(header->name[i].key);                                             \
      if (header->name[i].value)                                               \
        free(header->name[i].value);                                           \
    }                                                                          \
    free(header->name);                                                        \
    header->name = NULL;                                                       \
    header->name##_count = 0;                                                  \
  }
  PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_KV_PAIRS
}
