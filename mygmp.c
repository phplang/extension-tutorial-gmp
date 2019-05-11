#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_random.h"

#include <gmp.h>

static gmp_randstate_t randstate;

static PHP_FUNCTION(mygmp_version) {
	php_printf("%s\n", gmp_version);
}

static PHP_FUNCTION(mygmp_get_version) {
	RETURN_STRING(gmp_version);
}

static PHP_FUNCTION(mygmp_add) {
	zend_string *retstr, *arg1, *arg2;
	mpz_t ret, num1, num2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &arg1, &arg2) == FAILURE) {
		return;
	}

	/* Parse text strings into GMP ints */
	mpz_inits(ret, num1, num2, NULL);
	if (mpz_set_str(num1, ZSTR_VAL(arg1), 0) ||
		mpz_set_str(num2, ZSTR_VAL(arg2), 0)) {
		mpz_clears(ret, num1, num2, NULL);
		RETURN_FALSE;
	}

	/* Perform the operation */
	mpz_add(ret, num1, num2);

	/* Marshal the sum to a string for output */
	retstr = zend_string_alloc(mpz_sizeinbase(ret, 10), 0);
	mpz_get_str(ZSTR_VAL(retstr), 10, ret);
	ZSTR_LEN(retstr) = strlen(ZSTR_VAL(retstr));

	/* Free memory and return */
	mpz_clears(ret, num1, num2, NULL);
	RETURN_STR(retstr);
}

static PHP_FUNCTION(mygmp_random_ints) {
	zend_ulong count;
	zend_ulong bits = sizeof(unsigned long) << 3;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &count, &bits) == FAILURE) {
		return;
	}

	if (count < 0) {
		php_error(E_WARNING, "Invalid number of random ints requested");
		RETURN_FALSE;
	}

	if ((bits < 1) || (bits > (sizeof(unsigned long) << 3))) {
		php_error(E_WARNING, "Invalid bitsize requested, using %ld instead",
				  sizeof(unsigned long) << 3);
	}

	array_init(return_value);
	while (count--) {
		unsigned long val = gmp_urandomb_ui(randstate, bits);
		add_next_index_long(return_value, val);
	}
}

static zend_function_entry mygmp_functions[] = {
	PHP_FE(mygmp_version, NULL)
	PHP_FE(mygmp_get_version, NULL)
	PHP_FE(mygmp_add, NULL)
	PHP_FE(mygmp_random_ints, NULL)
	PHP_FE_END
};

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
    mygmp_functions,
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
