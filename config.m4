dnl config.m4
PHP_ARG_WITH(mygmp, for GNU MP support,
[  --with-mygmp[=DIR]          Include GNU MP support])

if test "$PHP_MYGMP" != "no"; then
  dnl Look for libgmp in explicit, or common implicit places.
  for i in $PHP_MYGMP /usr/local /usr; do
    if test -f $i/include/gmp.h; then
      MYGMP_DIR=$i
      break
    fi
  done

  if test -z "$MYGMP_DIR"; then
    AC_MSG_ERROR(Unable to locate gmp.h)
  fi

  dnl Update library list and include paths for libgmp
  PHP_ADD_LIBRARY_WITH_PATH(gmp, $MYGMP_DIR/$PHP_LIBDIR, MYGMP_SHARED_LIBADD)
  PHP_ADD_INCLUDE($MYGMP_DIR/include)
  PHP_SUBST(MYGMP_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mygmp, mygmp.c, $ext_shared)
fi
