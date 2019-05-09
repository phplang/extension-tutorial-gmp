dnl config.m4
PHP_ARG_ENABLE(mygmp)

if test "$PHP_MYGMP" != "no"; then
  PHP_NEW_EXTENSION(mygmp, mygmp.c, $ext_shared)
fi
