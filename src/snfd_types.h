#ifndef SNFD_TYPES_H
#define SNFD_TYPES_H

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


// Errors

#define SNFD_ERROR SNFD_UINT16
#define SNFD_ERROR_NO_ERROR 0
#define SNFD_ERROR_INVALID_PARAM 1

// Direct functions
typedef SNFD_ERROR (*SNFD_DIRECT_WRITE_FUNC) (SNFD_UINT32 destination, SNFD_UINT8 * source_buffer, SNFD_UINT32 count);

typedef struct {
    snfd_direct_write_func direct_write_func;
} snfd_device;

#endif /* end of include guard: SNFD_TYPES_H */
