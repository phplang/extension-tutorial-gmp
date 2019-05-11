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

static void do_mygmp_add(zval *return_value, zend_string *arg1, zend_string *arg2) {
	zend_string *retstr;
	mpz_t ret, num1, num2;

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

static PHP_FUNCTION(mygmp_add) {
	zend_string *arg1, *arg2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &arg1, &arg2) == FAILURE) {
		return;
	}

	do_mygmp_add(return_value, arg1, arg2);
}

static PHP_FUNCTION(mygmp_add_array) {
	zend_array *arr;
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &arr) == FAILURE) {
		return;
	}

	a = zend_symtable_str_find(arr, "a", strlen("a"));
	b = zend_symtable_str_find(arr, "b", strlen("b"));
	if (!a || (Z_TYPE_P(a) != IS_STRING) || !b || (Z_TYPE_P(b) != IS_STRING)) {
		php_error(E_WARNING, "Invalid or missing elements");
		return;
	}

	do_mygmp_add(return_value, Z_STR_P(a), Z_STR_P(b));
}

static PHP_FUNCTION(mygmp_sum) {
	zend_array *arr;
	mpz_t ret;
	zend_string *retstr;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &arr) == FAILURE) {
		return;
	}

	mpz_init(ret);

	/* foreach ($arr as $val) */
	ZEND_HASH_FOREACH_VAL(arr, val) {
		mpz_t v;
		zend_string *str = zval_get_string(val);

		mpz_init(v);
		if (mpz_set_str(v, ZSTR_VAL(str), 0)) {
			php_error(E_WARNING, "Invalid value encountered: %.*s", (int)ZSTR_LEN(str), ZSTR_VAL(str));
			mpz_clear(v);
			zend_string_release(str);
			continue;
		}

		mpz_add(ret, ret, v);
		mpz_clear(v);
		zend_string_release(str);
	} ZEND_HASH_FOREACH_END();

	retstr = zend_string_alloc(mpz_sizeinbase(ret, 10), 0);
	mpz_get_str(ZSTR_VAL(retstr), 10, ret);
	ZSTR_LEN(retstr) = strlen(ZSTR_VAL(retstr));

	mpz_clear(ret);
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
	PHP_FE(mygmp_add_array, NULL)
	PHP_FE(mygmp_sum, NULL)
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
