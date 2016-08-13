#ifndef MFFS_H
#define MFFS_H

#include "snfd_types.h"




SNFD_ERROR snfd_set_direct_write_func
(
		SNFD_DEVICE * device,
		SNFD_DIRECT_WRITE_FUNC write_func
);



#endif /* end of include guard: MFFS_H */
