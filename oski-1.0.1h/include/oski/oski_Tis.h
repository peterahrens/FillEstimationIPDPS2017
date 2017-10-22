/**
 *  \file include/oski/oski_Tis.h
 *  \brief Maps the default OSKI interface names to (int, float).
 */

#if defined(OSKI_REBIND)
/** Remove all bindings */
#  define OSKI_UNBIND
#  include <oski/oski.h>
#endif

/* Rebind */
#undef DEF_IND_TYPE
#undef DEF_VAL_TYPE
#define DEF_IND_TYPE 1 /**< Set default index type to 'int' */
#define DEF_VAL_TYPE 1 /**< Set default value type to 'float' */
#include <oski/oski.h>

/* eof */
