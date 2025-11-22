#ifndef UNIFIED_LIBCROCO
#define UNIFIED_LIBCROCO

#include <stdint.h>
#if INTPTR_MAX == INT32_MAX
    #include "libcroco_32.h"
#elif INTPTR_MAX == INT64_MAX
    #include "libcroco.h"
#endif

#endif
