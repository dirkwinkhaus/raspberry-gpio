/* widigpio extension for PHP */

#ifndef PHP_WIDIGPIO_H
# define PHP_WIDIGPIO_H

extern zend_module_entry widigpio_module_entry;
# define phpext_widigpio_ptr &widigpio_module_entry

PHP_METHOD(Gpio, setExport);
PHP_METHOD(Gpio, setDirection);
PHP_METHOD(Gpio, setIo);
PHP_METHOD(Gpio, getIo);
PHP_METHOD(Gpio, getOneWireDeviceList);
PHP_METHOD(Gpio, getOneWireDeviceValue);

# if defined(ZTS) && defined(COMPILE_DL_WIDIGPIO)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_WIDIGPIO_H */
