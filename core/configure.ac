AC_INIT([novemberizing-zip], [0.0.1], [novemberizing@gmail.com])

dnl --------------------------------------------------------------
dnl CHECK EMSCRIPTEN
dnl --------------------------------------------------------------
AS_CASE([$CC], [*emcc], [enable_emscripten="yes"], [enable_emscripten="no"])
AM_CONDITIONAL([EMSCRIPTEN], [test "x$enable_emscripten" = "xyes"])

AC_PROG_CC
AM_PROG_AR

dnl Setup for automake
AM_INIT_AUTOMAKE

LT_INIT([disable-shared])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_OUTPUT
