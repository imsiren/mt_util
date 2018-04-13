dnl $Id$
dnl config.m4 for extension mt

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(mt, for mt support,
Make sure that the comment is aligned:
[  --with-mt             Include mt support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(mt, whether to enable mt support,
dnl Make sure that the comment is aligned:
dnl [  --enable-mt           Enable mt support])

if test "$PHP_MT" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mt -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/mt.h"  # you most likely want to change this
  dnl if test -r $PHP_MT/$SEARCH_FOR; then # path given as parameter
  dnl   MT_DIR=$PHP_MT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for mt files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the mt distribution])
  dnl fi

  dnl # --with-mt -> add include path
  dnl PHP_ADD_INCLUDE($MT_DIR/include)

  dnl # --with-mt -> check for lib and symbol presence
  dnl LIBNAME=mt # you may want to change this
  dnl LIBSYMBOL=mt # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MT_DIR/$PHP_LIBDIR, MT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong mt lib version or lib not found])
  dnl ],[
  dnl   -L$MT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(MT_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mt,
  mt.c  \
  mt_init.c \    
  mt_logger.c ,
  $ext_shared)
fi
