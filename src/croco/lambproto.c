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

char *escape_string(const char *input) {
  if (!input)
    return NULL;

  // Calculate required length
  size_t len = 0;
  for (const char *p = input; *p; p++) {
    if (*p == '\\' || *p == '|' || *p == '=' || *p == ',') {
      len += 2; // character + escape
    } else {
      len += 1;
    }
  }

  // Allocate and escape
  char *result = malloc(len + 1);
  if (!result)
    return NULL;

  char *out = result;
  for (const char *p = input; *p; p++) {
    if (*p == '\\' || *p == '|' || *p == '=' || *p == ',') {
      *out++ = '\\';
      *out++ = *p;
    } else {
      *out++ = *p;
    }
  }
  *out = '\0';

  return result;
}

char *unescape_string(const char *input, size_t len) {
  if (len == 0) {
    char *result = malloc(1);
    if (result)
      result[0] = '\0';
    return result;
  }

  // Clculate required length
  size_t output_len = 0;
  for (size_t i = 0; i < len; i++) {
    if (input[i] == '\\' && i + 1 < len) {
      // Skip the backslash, count the escaped character
      i++;
    }
    output_len++;
  }

  // Allocate and unescape
  char *result = malloc(output_len + 1);
  if (!result)
    return NULL;

  char *out = result;
  for (size_t i = 0; i < len; i++) {
    if (input[i] == '\\' && i + 1 < len) {
      // Copy the escaped character
      i++;
      *out++ = input[i];
    } else {
      *out++ = input[i];
    }
  }
  *out = '\0';

  return result;
}

bool parse_key_value_pairs(const char *data, size_t len, KeyValuePair **pairs,
                           int *count) {
  if (len == 0) {
    *pairs = NULL;
    *count = 0;
    return true;
  }

  // Count key-value pairs (respecting escaped commas)
  size_t pair_count = 1;
  for (size_t i = 0; i < len; i++) {
    if (data[i] == '\\' && i + 1 < len) {
      i++; // Skip escaped character
    } else if (data[i] == ',') {
      pair_count++;
    }
  }

  KeyValuePair *result = malloc(pair_count * sizeof(KeyValuePair));
  if (!result)
    return false;

  // Parse each pair
  size_t start = 0;
  size_t pair_index = 0;

  for (size_t i = 0; i <= len; i++) {
    if (i == len || (data[i] == ',' && !(i > 0 && data[i - 1] == '\\'))) {
      // Found a pair boundary
      size_t pair_len = i - start;

      if (pair_len > 0) {
        // Find the first unescaped equals sign
        size_t equal_pos = (size_t)-1;
        for (size_t j = start; j < start + pair_len; j++) {
          if (data[j] == '\\' && j + 1 < start + pair_len) {
            j++; // Skip escaped character
          } else if (data[j] == '=') {
            equal_pos = j;
            break;
          }
        }

        if (equal_pos != (size_t)-1) {
          // Key and value both present
          size_t key_len = equal_pos - start;
          size_t value_len = (start + pair_len) - equal_pos - 1;

          result[pair_index].key = unescape_string(data + start, key_len);
          result[pair_index].value =
              unescape_string(data + equal_pos + 1, value_len);
        } else {
          // Only key present
          result[pair_index].key = unescape_string(data + start, pair_len);
          result[pair_index].value = NULL;
        }

        if (!result[pair_index].key) {
          // Cleanup on failure
          for (size_t j = 0; j < pair_index; j++) {
            free(result[j].key);
            free(result[j].value);
          }
          free(result);
          return false;
        }

        pair_index++;
      }

      start = i + 1; // Move past the comma
    }
  }

  *pairs = result;
  *count = pair_index;
  return true;
}

// Serialize header to string with escaped character support
char *CrocoHeader_serialize(const CrocoHeader *header) {
  if (!header)
    return NULL;

  // Calculate required buffer size
  size_t total_size = 1; // Null terminator

#define FIELD_STRING(name, _)                                                  \
  if (header->name) {                                                          \
    char *escaped = escape_string(header->name);                               \
    if (escaped) {                                                             \
      total_size += strlen(escaped);                                           \
      free(escaped);                                                           \
    }                                                                          \
  }                                                                            \
  total_size += 1; /* for '|' */
#define FIELD_KV_PAIRS(name, _)                                                \
  if (header->name && header->name##_count > 0) {                              \
    for (int i = 0; i < header->name##_count; i++) {                           \
      if (header->name[i].key) {                                               \
        char *escaped_key = escape_string(header->name[i].key);                \
        if (escaped_key) {                                                     \
          total_size += strlen(escaped_key);                                   \
          free(escaped_key);                                                   \
        }                                                                      \
      }                                                                        \
      total_size += 1; /* for '=' */                                           \
      if (header->name[i].value) {                                             \
        char *escaped_value = escape_string(header->name[i].value);            \
        if (escaped_value) {                                                   \
          total_size += strlen(escaped_value);                                 \
          free(escaped_value);                                                 \
        }                                                                      \
      }                                                                        \
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
    char *escaped = escape_string(header->name);                               \
    if (escaped) {                                                             \
      size_t len = strlen(escaped);                                            \
      memcpy(cursor, escaped, len);                                            \
      cursor += len;                                                           \
      free(escaped);                                                           \
    }                                                                          \
  }                                                                            \
  *cursor++ = '|';
#define FIELD_KV_PAIRS(name, _)                                                \
  if (header->name && header->name##_count > 0) {                              \
    for (int i = 0; i < header->name##_count; i++) {                           \
      if (header->name[i].key) {                                               \
        char *escaped_key = escape_string(header->name[i].key);                \
        if (escaped_key) {                                                     \
          size_t len = strlen(escaped_key);                                    \
          memcpy(cursor, escaped_key, len);                                    \
          cursor += len;                                                       \
          free(escaped_key);                                                   \
        }                                                                      \
      }                                                                        \
      *cursor++ = '=';                                                         \
      if (header->name[i].value) {                                             \
        char *escaped_value = escape_string(header->name[i].value);            \
        if (escaped_value) {                                                   \
          size_t len = strlen(escaped_value);                                  \
          memcpy(cursor, escaped_value, len);                                  \
          cursor += len;                                                       \
          free(escaped_value);                                                 \
        }                                                                      \
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

// Deserialize string to header with escaped character support
bool CrocoHeader_deserialize(CrocoHeader *header, const char *data) {
  if (!header || !data)
    return false;

  // Initialize header to clean state
  CrocoHeader_init(header);

  const char *cursor = data;
  int field_index = 0;

  while (*cursor) {
    // Find field end, respecting escaped pipes
    const char *field_end = NULL;
    for (const char *p = cursor; *p; p++) {
      if (*p == '\\' && *(p + 1)) {
        p++; // Skip escaped character
      } else if (*p == '|') {
        field_end = p;
        break;
      }
    }
    if (!field_end)
      field_end = cursor + strlen(cursor);

    size_t field_len = field_end - cursor;

    // Use the macro to handle field deserialization
    bool field_processed = false;

#define FIELD_STRING(name, idx)                                                \
  if (field_index == idx) {                                                    \
    if (field_len > 0) {                                                       \
      header->name = unescape_string(cursor, field_len);                       \
      if (!header->name) {                                                     \
        CrocoHeader_free(header);                                              \
        return false;                                                          \
      }                                                                        \
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
