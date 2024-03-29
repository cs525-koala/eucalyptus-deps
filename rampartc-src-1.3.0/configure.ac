dnl run autogen.sh to generate the configure script.

AC_PREREQ(2.59)

AC_INIT(rampartc-src, 1.3.0)
AC_CANONICAL_SYSTEM
AM_CONFIG_HEADER(config.h)
dnl AM_INIT_AUTOMAKE([tar-ustar])
AM_INIT_AUTOMAKE
m4_ifdef([_A][M_PROG_TAR],[_A][M_SET_OPTION([tar-ustar])])
AC_PREFIX_DEFAULT(/usr/local/rampartc)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl check for flavours of varargs macros (test from GLib)
AC_MSG_CHECKING(for ISO C99 varargs macros in C)
AC_TRY_COMPILE([],[
int a(int p1, int p2, int p3);
#define call_a(...) a(1,__VA_ARGS__)
call_a(2,3);
],axis2c_have_iso_c_varargs=yes,axis2c_have_iso_c_varargs=no)
AC_MSG_RESULT($axis2c_have_iso_c_varargs)

AC_MSG_CHECKING(for GNUC varargs macros)
AC_TRY_COMPILE([],[
int a(int p1, int p2, int p3);
#define call_a(params...) a(1,params)
call_a(2,3);
],axis2c_have_gnuc_varargs=yes,axis2c_have_gnuc_varargs=no)
AC_MSG_RESULT($axis2c_have_gnuc_varargs)

dnl Output varargs tests
if test x$axis2c_have_iso_c_varargs = xyes; then
    AC_DEFINE(HAVE_ISO_VARARGS,1,[Have ISO C99 varargs macros])
fi
if test x$axis2c_have_gnuc_varargs = xyes; then
    AC_DEFINE(HAVE_GNUC_VARARGS,1,[Have GNU-style varargs macros])
fi

dnl Checks for libraries.
AC_CHECK_LIB(dl, dlopen)

CFLAGS="$CFLAGS -D_LARGEFILE64_SOURCE -g3"
if test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS -ansi -Wall -Werror -ggdb -Wno-implicit-function-declaration"
    #CFLAGS="$CFLAGS -ansi -Wall -Wno-implicit-function-declaration"
fi

LDFLAGS="$LDFLAGS -lpthread"

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stdlib.h string.h])

dnl Checks for typedefs, structures, and compiler characteristics.
dnl AC_C_CONST

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
#AC_CHECK_FUNCS([memmove])

AC_MSG_CHECKING(path to use Axis2C . This is a compulsory to build Rampart-C)
AC_ARG_WITH(axis2,
[  --with-axis2[=PATH]      use axis2c.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    ;;
  *)
    AC_MSG_RESULT($withval)
    dnl Find axis2 include dir in the path
    if test -d $withval; then
        axis2inc="-I$withval"
    dnl else find the axis2inc include dir in $(AXIS2C_HOME)/include
    elif test -d '$(AXIS2C_HOME)/include/axis2-1.6.0'; then
        axis2inc="-I$(AXIS2C_HOME)/include/axis2-1.6.0"
    else
        AC_MSG_ERROR(could not find axis2inc. stop)
    fi
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(path to use openssl . This is a compulsory to build Rampart-C)
AC_ARG_WITH(openssl,
[  --with-openssl[=PATH]      use openssl.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    ;;
  *)
    AC_MSG_RESULT($withval)
    dnl Find oprnssl dir in the path
    if test -d $withval; then
        opensslinc="-I$withval/include"
        openssllib="-L$withval/lib"
    elif test -d '/usr/include/openssl'; then
        opensslinc="-I/usr/include/openssl"
        openssllib=""
    else
        AC_MSG_ERROR(could not find openssl. stop)
    fi
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

AXIS2INC=$axis2inc
AXIS2LIB='-L$(AXIS2C_HOME)/lib'
AXIOMINC=$axiominc
NEETHIINC=$neethiinc
OPENSSLINC=$opensslinc
OPENSSLLIB=$openssllib
UTILINC=$axis2_utilinc
VERSION_NO="3:0:3"
AC_SUBST(AXIS2INC)
AC_SUBST(AXIS2LIB)
AC_SUBST(AXIOMINC)
AC_SUBST(OPENSSLINC)
AC_SUBST(OPENSSLLIB)
AC_SUBST(NEETHIINC)
AC_SUBST(UTILINC)
AC_SUBST(XMLSCHEMAINC)
AC_SUBST(VERSION_NO)


AC_CONFIG_FILES([Makefile \
    src/Makefile \
    src/omxmlsec/Makefile \
    src/omxmlsec/tokens/Makefile \
    src/omxmlsec/openssl/Makefile \
    src/omxmlsec/c14n/Makefile \
    src/data/Makefile \
    src/handlers/Makefile \
    src/core/Makefile \
    src/trust/Makefile \
    src/util/Makefile \
    src/secconv/Makefile \
    src/rahas/Makefile \
    test/Makefile \
    test/omxmlsec/Makefile \
    test/c14n/Makefile \
    test/openssl/Makefile 
    test/openssl/sign/Makefile 
    ])
    
AC_OUTPUT
