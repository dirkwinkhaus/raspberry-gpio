/* widigpio extension for PHP */

#ifndef PHP_WIDIGPIO_H
# define PHP_WIDIGPIO_H

extern zend_module_entry widigpio_module_entry;
# define phpext_widigpio_ptr &widigpio_module_entry

# define PHP_WIDIGPIO_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_WIDIGPIO)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_WIDIGPIO_H */
