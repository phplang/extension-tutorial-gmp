#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

zend_module_entry mygmp_module_entry = {
    STANDARD_MODULE_HEADER,
    "mygmp",
    NULL, /* functions */
    NULL, /* MINIT */
    NULL, /* MSHUTDOWN */
    NULL, /* RINIT */
    NULL, /* RSHUTDOWN */
    NULL, /* MINFO */
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MYGMP
ZEND_GET_MODULE(mygmp)
#endif
