AC_PREREQ(2.52)
AC_INIT([genesis], [2.4], [genesis@genesis-sim.org])

AC_PREFIX_DEFAULT(/usr/local/$PACKAGE_NAME)

# If the user did not supply --prefix, we fail loudly
# (modified from NEST's configure.ac)
if test $prefix = NONE; then
  echo "No valid installation prefix supplied! Defaulting to $ac_default_prefix."
  echo "To install into a custom path, you need to run ./configure --prefix=<path>"
  echo "Example: to install $PACKAGE_NAME into ~/opt/$PACKAGE_NAME-$PACKAGE_VERSION, run the following:"
  echo "\t./configure --prefix=\$HOME/opt/$PACKAGE_NAME-$PACKAGE_VERSION"
fi


# check for correct compiler & libraries
AC_PROG_CC
AC_PROG_CPP
AC_PATH_X
AC_PATH_XTRA
AC_PROG_RANLIB
AC_PROG_YACC
if test x"$YACC" = "xyacc"; then
  AC_CHECK_PROG([YACC_EXISTS], [yacc], [yes], [no])
  if test x"$YACC_EXISTS" != xyes; then
    AC_MSG_ERROR([[bison|byacc|yacc not found. Please install _bison_ in order to compile Genesis.]])
  fi
fi
AC_PROG_LEX
if test x"$LEX" == x":" ; then
    AC_MSG_ERROR([flex|lex not found. Please install _flex_ in order to compile Genesis.])
fi


AC_CHECK_LIB([ncurses], [main], [], [AC_MSG_ERROR([Missing ncurses.])])

AC_SUBST(X_INC, $x_includes)
AC_SUBST(X_LIB, $x_libraries)
AC_SUBST(LEX, $LEX)
AC_SUBST(LEXLIB, $LEXLIB)

AC_CHECK_SIZEOF([long int])
AS_IF([test "$ac_cv_sizeof_long_int" -eq 8],
    [AC_DEFINE([LONGWORDS])])

AC_OUTPUT(Makefile)
