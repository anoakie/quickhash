/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
 */
/* $Id: quickhash.c,v 1.40 2009-03-30 18:36:55 derick Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_quickhash.h"
#include "php_globals.h"

function_entry quickhash_functions[] = {
	{NULL, NULL, NULL}
};


zend_module_entry quickhash_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"quickhash",
	quickhash_functions,
	PHP_MINIT(quickhash),
	PHP_MSHUTDOWN(quickhash),
	PHP_RINIT(quickhash),	
	PHP_RSHUTDOWN(quickhash),
	PHP_MINFO(quickhash),
#if ZEND_MODULE_API_NO >= 20010901
	"0.0.1",
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_QUICKHASH
ZEND_GET_MODULE(quickhash)
#endif

ZEND_DECLARE_MODULE_GLOBALS(quickhash)
/*
PHP_INI_BEGIN()
PHP_INI_END()
*/
 
static void quickhash_init_globals(zend_quickhash_globals *quickhash_globals)
{
	/* Empty */
}


PHP_MINIT_FUNCTION(quickhash)
{
	ZEND_INIT_MODULE_GLOBALS(quickhash, quickhash_init_globals, NULL);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(quickhash)
{
	return SUCCESS;
}



PHP_RINIT_FUNCTION(quickhash)
{
	return SUCCESS;
}



PHP_RSHUTDOWN_FUNCTION(quickhash)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(quickhash)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "quickhash support", "enabled");
	php_info_print_table_end();
}
