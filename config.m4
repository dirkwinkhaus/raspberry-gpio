PHP_ARG_ENABLE(widigpio, whether to enable widigpio support,
[  --enable-widigpio          Enable widigpio support], no)

if test "$PHP_WIDIGPIO" != "no"; then
  AC_DEFINE(HAVE_WIDIGPIO, 1, [ Have widigpio support ])
  PHP_NEW_EXTENSION(widigpio, widigpio.c, $ext_shared)
fi
