#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_random.h"

#include <gmp.h>

static gmp_randstate_t randstate;

static PHP_MINIT_FUNCTION(mygmp) {
	zend_ulong seed;

	if (FAILURE == php_random_int_silent(0, ZEND_ULONG_MAX, &seed)) {
		return FAILURE;
	}

	gmp_randinit_mt(randstate);
	gmp_randseed_ui(randstate, seed);

	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(mygmp) {
	gmp_randclear(randstate);

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(mygmp) {
    php_info_print_table_start();
    php_info_print_table_row(2, "MyGMP support", "enabled");
    php_info_print_table_row(2, "libgmp version", gmp_version);
    php_info_print_table_end();
}

zend_module_entry mygmp_module_entry = {
    STANDARD_MODULE_HEADER,
    "mygmp",
    NULL, /* functions */
    PHP_MINIT(mygmp),
	PHP_MSHUTDOWN(mygmp),
    NULL, /* RINIT */
    NULL, /* RSHUTDOWN */
    PHP_MINFO(mygmp),
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MYGMP
ZEND_GET_MODULE(mygmp)
#endif
