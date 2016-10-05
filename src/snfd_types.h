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
#define SNFD_ERROR_NO_SPACE_LEFT 2
#define SNFD_ERROR_FILE_NOT_FOUND 3

// Block states
// Available transitions
// FREE -> CLEAN
// CLEAN -> DIRTY
// DIRTY -> FREE (erase needed)
// * -> BROKEN

#define SNFD_BLOCK_UNUSED_1 0xFF
#define SNFD_BLOCK_FREE 0xFE
#define SNFD_BLOCK_CLEAN 0xFC
#define SNFD_BLOCK_DIRTY 0xF8
#define SNFD_BLOCK_BROKEN 0xF0
#define SNFD_BLOCK_UNUSED_2 0xE0
#define SNFD_BLOCK_UNUSED_3 0xC0
#define SNFD_BLOCK_UNUSED_4 0x80
#define SNFD_BLOCK_UNUSED_5 0x00

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


typedef SNFD_UINT8 SNFD_BLOCK_STATE;

// Block header struct

typedef struct {
    SNFD_UINT8 magic_number[SNFD_MAGIC_NUMBER_SIZE];
    SNFD_BLOCK_STATE state;
} __attribute__((packed)) SNFD_BLOCK_HEADER;

// Block struct
typedef struct {
    SNFD_BLOCK_STATE state; // FREE, DIRTY, CLEAN or BROKEN
} SNFD_BLOCK;

// Main struct
typedef struct {
    SNFD_CONFIG config;
    SNFD_UINT8 buffer[SNFD_EXCHANGE_BUFFER_SIZE];
    SNFD_BLOCK blocks[SNFD_BLOCKS_COUNT];
} SNFD;

typedef SNFD_UINT16 SNFD_FILE_NUMBER;
typedef SNFD_UINT8 SNFD_FILE_OPERATION;

//File operations
#define SNFD_LOG_OPERATION_UNUSED_1 0xFF
#define SNFD_LOG_OPERATION_SET 0xFE
#define SNFD_LOG_OPERATION_UNUSED_2 0xFC
#define SNFD_LOG_OPERATION_UNUSED_3 0xF8
#define SNFD_LOG_OPERATION_UNUSED_4 0xF0
#define SNFD_LOG_OPERATION_UNUSED_5 0xE0
#define SNFD_LOG_OPERATION_UNUSED_6 0xC0
#define SNFD_LOG_OPERATION_UNUSED_7 0x80
#define SNFD_LOG_OPERATION_UNUSED_8 0x00

//Log state
#define SNFD_LOG_INVALID 0xFF
#define SNFD_LOG_ACTIVE 0xFE
#define SNFD_LOG_OBSOLETE 0xFC
#define SNFD_LOG_UNUSED_2 0xF8
#define SNFD_LOG_UNUSED_3 0xF0
#define SNFD_LOG_UNUSED_4 0xE0
#define SNFD_LOG_UNUSED_5 0xC0
#define SNFD_LOG_UNUSED_6 0x80
#define SNFD_LOG_UNUSED_7 0x00

// Log struct
typedef struct {
    SNFD_FILE_NUMBER file_number;
    SNFD_UINT32 order_number;
    SNFD_FILE_OPERATION file_operation;
    SNFD_UINT8 state;
    SNFD_UINT32 start_loc;
    SNFD_UINT32 data_size;
} __attribute__((packed)) SNFD_LOG;


#endif /* end of include guard: SNFD_TYPES_H */
