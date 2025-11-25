#ifndef LIBLAMBP_H
#define LIBLAMBP_H

#include <stdbool.h>

// Protocol field macros for easy extension
#define PROTOCOL_FIELDS \
    FIELD_STRING(protocolName, 0) \
    FIELD_STRING_ARRAY(extraOptFlags, 1)

// Field type definitions
typedef enum {
    FIELD_STRING,
    FIELD_STRING_ARRAY
} FieldType;

// Field structure
typedef struct {
    const char* key;
    FieldType type;
    void* value;
} ProtocolField;

// Main protocol structure
typedef struct {
#define FIELD_STRING(name, _) char* name;
#define FIELD_STRING_ARRAY(name, _) char** name;
    PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_STRING_ARRAY
} CrocoHeader;

// Serialization function
char* CrocoHeader_serialize(const CrocoHeader* header);
bool CrocoHeader_deserialize(CrocoHeader* header, const char* data);

#endif
