#ifndef LIBLAMBP_H
#define LIBLAMBP_H

#include <stdbool.h>

#define PROTOCOL_FIELDS \
    FIELD_STRING(protocolName, 0) \
    FIELD_KV_PAIRS(extraOptFlags, 1)

// Field type definitions
typedef enum {
    FIELD_STRING,
    FIELD_KV_PAIRS
} FieldType;

typedef struct {
    char* key;
    char* value;
} KeyValuePair;

// Main protocol structure
typedef struct {
#define FIELD_STRING(name, _) char* name;
#define FIELD_KV_PAIRS(name, _) KeyValuePair* name; int name##_count;
    PROTOCOL_FIELDS
#undef FIELD_STRING
#undef FIELD_KV_PAIRS
} CrocoHeader;

// Serialization/deserialization functions
char* CrocoHeader_serialize(const CrocoHeader* header);
bool CrocoHeader_deserialize(CrocoHeader* header, const char* data);
void CrocoHeader_free(CrocoHeader* header);
void CrocoHeader_init(CrocoHeader* header);

#endif
