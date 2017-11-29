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

/* {{{ void gpio_export(int port, bool export)
 */
PHP_FUNCTION(gpio_export)
{
    long port;
    int fd;
    zend_bool export;
    ssize_t bytes_written;
    char buffer[3];
    char file_name[24];

    ZEND_PARSE_PARAMETERS_START(2, 2)
      Z_PARAM_LONG(port)
      Z_PARAM_BOOL(export)
    ZEND_PARSE_PARAMETERS_END();
	
	memset(file_name, 0, sizeof(file_name));	
	
	if (export) {
		strcat(file_name, "/sys/class/gpio/export");
	} else {
		strcat(file_name, "/sys/class/gpio/unexport");
	}
	
	fd = open(file_name, O_WRONLY);
	if (-1 == fd) {
    	RETURN_FALSE;
	}
 
	bytes_written = snprintf(buffer, 3, "%ld", port);
	
	if (-1 == write(fd, buffer, bytes_written)) {
		RETURN_FALSE;	
	}
	
	close(fd);

	RETURN_TRUE;
}
/* }}} */

/* {{{ bool gpio_direction( int $port, bool $direction )
 */
PHP_FUNCTION(gpio_direction)
{
	long port;
	zend_bool direction;
	char direction_str[3];
 	char path[35];
	int fd;

    ZEND_PARSE_PARAMETERS_START(2, 2)
      Z_PARAM_LONG(port)
      Z_PARAM_BOOL(direction)
    ZEND_PARSE_PARAMETERS_END();
	
	snprintf(path, 35, "/sys/class/gpio/gpio%ld/direction", port);
	
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		RETURN_FALSE;
	}
	
	if (direction) {
		strcat(direction_str, "out");
	} else {
		strcat(direction_str, "in");
	}
 
	if (-1 == write(fd, direction_str, strlen(direction_str))) {
		RETURN_FALSE;
	}
 
	close(fd);

	RETURN_TRUE;
}
/* }}}*/

/* {{{ bool gpio_set( int $port, bool $set )
 */
PHP_FUNCTION(gpio_set)
{
	long port;
	zend_bool set;
	char path[35];
	char set_str[1];
	int fd;

    ZEND_PARSE_PARAMETERS_START(2, 2)
      Z_PARAM_LONG(port)
      Z_PARAM_BOOL(set)
    ZEND_PARSE_PARAMETERS_END();
 
	snprintf(path, 35, "/sys/class/gpio/gpio%ld/value", port);
	
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		RETURN_FALSE;
	}
	
	if (set) {
		strcat(set_str, "1");
	} else {
		strcat(set_str, "0");
	}
 
	if (1 != write(fd, set_str, 1)) {
		fprintf(stderr, "Failed to write value!\n");
		RETURN_FALSE;
	}
 
	close(fd);

	RETURN_TRUE;
}
/* }}}*/

/* {{{ bool gpio_get( int $port )
 */
PHP_FUNCTION(gpio_get)
{
	long port;
	char path[35];
	char input[1];
	int fd;

    ZEND_PARSE_PARAMETERS_START(1, 1)
      Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END();
 
	snprintf(path, 35, "/sys/class/gpio/gpio%ld/value", port);
	
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		RETURN_NULL();
	}
 
	if (-1 == read(fd, input, 1)) {
		fprintf(stderr, "Failed to read value!\n");
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

/* {{{ bool gpio_onewire_slaves_list( )
 */
PHP_FUNCTION(gpio_onewire_slaves_list)
{
    char input_string[1024];
    char *part_string;
	int fd;

    ZEND_PARSE_PARAMETERS_START(0, 0)
    ZEND_PARSE_PARAMETERS_END();
	
	fd = open("/sys/bus/w1/devices/w1_bus_master1/w1_master_slaves", O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open w1 bus master!\n");
		RETURN_NULL();
	}
 
	if (-1 == read(fd, input_string, 1024)) {
		fprintf(stderr, "cannot read bus master!\n");
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

/* {{{ int gpio_onewire_slave_value( string deviceName )
 */
PHP_FUNCTION(gpio_onewire_slave_value)
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
		fprintf(stderr, "Failed to open w1 bus master!\n");
		RETURN_NULL();
	}
 
	if (-1 == read(fd, input_string, 1024)) {
		fprintf(stderr, "cannot read bus master!\n");
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
	PHP_FE(gpio_export,	arginfo_gpio_export)
	PHP_FE(gpio_direction, arginfo_gpio_direction)
	PHP_FE(gpio_set, arginfo_gpio_set)
	PHP_FE(gpio_get, arginfo_gpio_get)
	PHP_FE(gpio_onewire_slaves_list, arginfo_gpio_onewire_slaves_list)
	PHP_FE(gpio_onewire_slave_value, arginfo_gpio_onewire_slave_value)
	PHP_FE_END
};
/* }}} */

/* {{{ widigpio_module_entry
 */
zend_module_entry widigpio_module_entry = {
	STANDARD_MODULE_HEADER,
	"widigpio",					/* Extension name */
	widigpio_functions,			/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(widigpio),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(widigpio),			/* PHP_MINFO - Module info */
	PHP_WIDIGPIO_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WIDIGPIO
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(widigpio)
#endif
