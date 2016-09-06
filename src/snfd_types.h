#ifndef SNFD_TYPES_H
#define SNFD_TYPES_H

#include "snfd_config.h"

// Basic types

#ifndef SNFD_INIT32
#define SNFD_INT32 int
#endif

#ifndef SNFD_UINT32
#define SNFD_UINT32 unsigned int
#endif

#ifndef SNFD_INT16
#define SNFD_INT16 short
#endif

#ifndef SNFD_UINT16
#define SNFD_UINT16 unsigned short
#endif

#ifndef SNFD_INT8
#define SNFD_INT8 char
#endif

#ifndef SNFD_UINT8
#define SNFD_UINT8 unsigned char
#endif

#ifndef SNFD_BOOL
#define SNFD_BOOL char
#endif

#define SNFD_TRUE 1
#define SNFD_FALSE 0


// Errors
#define SNFD_ERROR SNFD_UINT16
#define SNFD_ERROR_NO_ERROR 0
#define SNFD_ERROR_INVALID_PARAM 1

// Block states
// Available transitions
// FREE -> CLEAN
// CLEAN -> DIRTY
// DIRTY -> FREE
#define SNFD_BLOCK_FREE 1
#define SNFD_BLOCK_CLEAN 3
#define SNFD_BLOCK_DIRTY 7

// Direct functions
typedef SNFD_ERROR (*SNFD_DIRECT_WRITE_FUNC) (SNFD_UINT32 destination, void * source, SNFD_UINT32 size);
typedef SNFD_ERROR (*SNFD_DIRECT_READ_FUNC) (SNFD_UINT32 source, void * destination, SNFD_UINT32 size);
typedef SNFD_ERROR (*SNFD_DIRECT_BLOCK_ERASE_FUNC) (SNFD_UINT16 block_number);

// SNFD configuration structure
typedef struct {
    SNFD_DIRECT_WRITE_FUNC write_func;
    SNFD_DIRECT_READ_FUNC read_func;
    SNFD_DIRECT_BLOCK_ERASE_FUNC block_erase_func;
} SNFD_CONFIG;


// Fragment flags
#define SNFD_FRAGMENT_FLAG_ROOT 1
#define SNFD_FRAGMENT_FLAG_DEPRECATED 2

// Fragment struct
typedef struct {
    SNFD_UINT32 file_id;
    SNFD_UINT8 operation;
    SNFD_UINT32 parent;
    SNFD_UINT8 flags;
} __attribute__((packed)) SNFD_FRAGMENT;

// Block header struct
typedef struct {
    SNFD_UINT8 magic_number[SNFD_MAGIC_NUMBER_SIZE];
    SNFD_UINT8 state;
} __attribute__((packed)) SNFD_BLOCK_HEADER;

// Block struct
typedef struct {
    SNFD_UINT32 state; // FREE, DIRTY or CLEAN
} SNFD_BLOCK;

// Main struct
typedef struct {
    SNFD_CONFIG config;
    SNFD_UINT8 buffer[SNFD_EXCHANGE_BUFFER_SIZE];
    SNFD_BLOCK blocks[SNFD_BLOCKS_COUNT];
} SNFD;

#endif /* end of include guard: SNFD_TYPES_H */
