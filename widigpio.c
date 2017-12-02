/* widigpio extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_widigpio.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

zend_class_entry *gpio_ce;

/* {{{ void setExport(int port, bool export)
 */
PHP_METHOD(Gpio, setExport)
{
    zend_long port;
    int fd;
    zend_bool export;
    ssize_t bytes_written;
    char buffer[3];
    char file_name[24];

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(port)
        Z_PARAM_BOOL(export)
    ZEND_PARSE_PARAMETERS_END();
    
    if (export) {
        strcpy(file_name, "/sys/class/gpio/export");
    } else {
        strcpy(file_name, "/sys/class/gpio/unexport");
    }
    
    fd = open(file_name, O_WRONLY);
    if (-1 == fd) {
        RETURN_FALSE;
    }
 
    bytes_written = snprintf(buffer, 3, "%pd", port);
    
    if (-1 == write(fd, buffer, bytes_written)) {
        RETURN_FALSE;    
    }
    
    close(fd);

    RETURN_TRUE;
}
/* }}} */

/* {{{ bool setDirection( int $port, bool $direction )
 */
PHP_METHOD(Gpio, setDirection)
{
    zend_long port;
    zend_bool direction;
    char direction_str[3];
     char path[35];
    int fd;

    ZEND_PARSE_PARAMETERS_START(2, 2)
      Z_PARAM_LONG(port)
      Z_PARAM_BOOL(direction)
    ZEND_PARSE_PARAMETERS_END();
    
    snprintf(path, 35, "/sys/class/gpio/gpio%pd/direction", port);
    
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        RETURN_FALSE;
    }
    
    if (direction) {
        strcpy(direction_str, "out");
    } else {
        strcpy(direction_str, "in");
    }
    
    if (-1 == write(fd, direction_str, strlen(direction_str))) {
        RETURN_FALSE;
    }
 
    close(fd);

    RETURN_TRUE;
}
/* }}}*/

/* {{{ bool setIo( int $port, bool $set )
 */
PHP_METHOD(Gpio, setIo)
{
    zend_long port;
    zend_bool set;
    char path[35];
    char set_str[1];
    int fd;

    ZEND_PARSE_PARAMETERS_START(2, 2)
      Z_PARAM_LONG(port)
      Z_PARAM_BOOL(set)
    ZEND_PARSE_PARAMETERS_END();
 
    snprintf(path, 35, "/sys/class/gpio/gpio%pd/value", port);
    
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        RETURN_FALSE;
    }
    
    if (set) {
        strcpy(set_str, "1");
    } else {
        strcpy(set_str, "0");
    }
 
    if (1 != write(fd, set_str, 1)) {
        RETURN_FALSE;
    }
 
    close(fd);

    RETURN_TRUE;
}
/* }}}*/

/* {{{ bool getIo( int $port )
 */
PHP_METHOD(Gpio, getIo)
{
    zend_long port;
    char path[35];
    char input[1];
    int fd;

    ZEND_PARSE_PARAMETERS_START(1, 1)
      Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END();
 
    snprintf(path, 35, "/sys/class/gpio/gpio%pd/value", port);
    
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
        RETURN_NULL();
    }
 	
 	memset(input,0,strlen(input));
    
    if (-1 == read(fd, input, 1)) {
        RETURN_NULL();
    }

    close(fd);

    if (strcmp(input, "1") == 0) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}
/* }}}*/

/* {{{ bool getOneWireDeviceList( )
 */
PHP_METHOD(Gpio, getOneWireDeviceList)
{
    char input_string[1024];
    char *part_string;
    int fd;

    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
    
    fd = open("/sys/bus/w1/devices/w1_bus_master1/w1_master_slaves", O_RDONLY);
    if (-1 == fd) {
        RETURN_NULL();
    }
 
    if (-1 == read(fd, input_string, 1024)) {
        RETURN_NULL();
    }

    close(fd);

    array_init(return_value);        
    part_string = strtok(input_string, "\n");
    
    if (part_string != NULL) {
        add_next_index_string(return_value, part_string);
        while ((part_string = strtok(NULL, "\n")) != NULL) {
            add_next_index_string(return_value, part_string);
        }
    }
}
/* }}}*/

/* {{{ int getOneWireDeviceValue( string deviceName )
 */
PHP_METHOD(Gpio, getOneWireDeviceValue)
{
    char *device_name;
    size_t device_name_len;
    char path[64];
    char input_string[1024];
    char *temp_string;
    long temp;
    int fd;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(device_name, device_name_len)
    ZEND_PARSE_PARAMETERS_END();
    
    snprintf(path, 64, "/sys/bus/w1/devices/%s/w1_slave", device_name);
    
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
        RETURN_NULL();
    }
 
    if (-1 == read(fd, input_string, 1024)) {
        RETURN_NULL();
    }
    
    close(fd);

    temp_string = strstr(input_string, "t=");
    memmove(temp_string, temp_string+2, strlen(temp_string));
    temp = atol(temp_string);

    RETURN_LONG(temp);
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(widigpio)
{
#if defined(ZTS) && defined(COMPILE_DL_WIDIGPIO)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(widigpio)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "widigpio support", "enabled");
    php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_gpio_export, 0)
    ZEND_ARG_INFO(0, int)
    ZEND_ARG_INFO(0, bool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gpio_direction, 0)
    ZEND_ARG_INFO(0, int)
    ZEND_ARG_INFO(0, bool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gpio_set, 0)
    ZEND_ARG_INFO(0, int)
    ZEND_ARG_INFO(0, bool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gpio_get, 0)
    ZEND_ARG_INFO(0, int)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gpio_onewire_slaves_list, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gpio_onewire_slave_value, 0)
    ZEND_ARG_INFO(0, char[])
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ widigpio_functions[]
 */
const zend_function_entry widigpio_functions[] = {
    PHP_ME(Gpio, setExport,    arginfo_gpio_export, ZEND_ACC_PUBLIC)
    PHP_ME(Gpio, setDirection, arginfo_gpio_direction, ZEND_ACC_PUBLIC)
    PHP_ME(Gpio, setIo, arginfo_gpio_set, ZEND_ACC_PUBLIC)
    PHP_ME(Gpio, getIo, arginfo_gpio_get, ZEND_ACC_PUBLIC)
    PHP_ME(Gpio, getOneWireDeviceList, arginfo_gpio_onewire_slaves_list, ZEND_ACC_PUBLIC)
    PHP_ME(Gpio, getOneWireDeviceValue, arginfo_gpio_onewire_slave_value, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

PHP_MINIT_FUNCTION(widigpio)
{
    zend_class_entry tmp_ce;
    INIT_CLASS_ENTRY(tmp_ce, "Gpio", widigpio_functions);

    gpio_ce = zend_register_internal_class(&tmp_ce TSRMLS_CC);

    return SUCCESS;
}

/* {{{ widigpio_module_entry
 */
zend_module_entry widigpio_module_entry = {
    STANDARD_MODULE_HEADER,
    "widigpio",                    /* Extension name */
    widigpio_functions,            /* zend_function_entry */
    PHP_MINIT(widigpio),        /* PHP_MINIT - Module initialization */
    NULL,                        /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(widigpio),        /* PHP_RINIT - Request initialization */
    NULL,                        /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(widigpio),        /* PHP_MINFO - Module info */
    PHP_WIDIGPIO_VERSION,        /* Version */
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WIDIGPIO
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(widigpio)
#endif
