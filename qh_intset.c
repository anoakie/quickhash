/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 StumbleUpon Inc.                                  |
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
/* $Id$ */

#include "php.h"
#include "zend.h"
#include "zend_API.h"
#include "qh_intset.h"
#include "quickhash.h"

zend_class_entry *qh_ce_intset;

PHPAPI zend_class_entry *php_qh_get_intset_ce(void)
{
	return qh_ce_intset;
}

zend_object_handlers qh_object_handlers_intset;

static void qh_object_free_storage_intset(void *object TSRMLS_DC);
static zend_object_value qh_object_new_intset(zend_class_entry *class_type TSRMLS_DC);

/* Reflection Information Structs */
ZEND_BEGIN_ARG_INFO_EX(arginfo_qh_intset_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, size)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_qh_intset_add, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_qh_intset_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_qh_intset_load_from_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_qh_intset_save_to_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_qh_intset_load_from_string, 0, 0, 1)
	ZEND_ARG_INFO(0, contents)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

/* Class methods definition */
zend_function_entry qh_funcs_intset[] = {
	PHP_ME(QuickHashIntSet, __construct,    arginfo_qh_intset_construct,        ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(QuickHashIntSet, add,            arginfo_qh_intset_add,              ZEND_ACC_PUBLIC)
	PHP_ME(QuickHashIntSet, exists,         arginfo_qh_intset_exists,           ZEND_ACC_PUBLIC)
	PHP_ME(QuickHashIntSet, loadFromFile,   arginfo_qh_intset_load_from_file,   ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	PHP_ME(QuickHashIntSet, saveToFile,     arginfo_qh_intset_save_to_file,     ZEND_ACC_PUBLIC)
	PHP_ME(QuickHashIntSet, loadFromString, arginfo_qh_intset_load_from_string, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void qh_register_class_intset(TSRMLS_D)
{
	zend_class_entry ce_intset;

	INIT_CLASS_ENTRY(ce_intset, "QuickHashIntSet", qh_funcs_intset);
	ce_intset.create_object = qh_object_new_intset;
	qh_ce_intset = zend_register_internal_class_ex(&ce_intset, NULL, NULL TSRMLS_CC);
	memcpy(&qh_object_handlers_intset, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	zend_declare_class_constant_long(qh_ce_intset, "CHECK_FOR_DUPES", sizeof("CHECK_FOR_DUPES") - 1, QH_NO_DUPLICATES TSRMLS_CC);
	zend_declare_class_constant_long(qh_ce_intset, "DO_NOT_USE_ZEND_ALLOC",  sizeof("DO_NOT_USE_ZEND_ALLOC") - 1, QH_DO_NOT_USE_ZEND_ALLOC TSRMLS_CC);
	zend_declare_class_constant_long(qh_ce_intset, "HASHER_NO_HASH",   sizeof("HASHER_NO_HASH") - 1,  QH_HASHER_NO_HASH TSRMLS_CC);
	zend_declare_class_constant_long(qh_ce_intset, "HASHER_JENKINS1",  sizeof("HASHER_JENKINS1") - 1, QH_HASHER_JENKINS1 TSRMLS_CC);
	zend_declare_class_constant_long(qh_ce_intset, "HASHER_JENKINS2",  sizeof("HASHER_JENKINS2") - 1, QH_HASHER_JENKINS2 TSRMLS_CC);
}

static inline zend_object_value qh_object_new_intset_ex(zend_class_entry *class_type, php_qh_intset_obj **ptr TSRMLS_DC)
{
	php_qh_intset_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_qh_intset_obj));
	memset(intern, 0, sizeof(php_qh_intset_obj));
	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) qh_object_free_storage_intset, NULL TSRMLS_CC);
	retval.handlers = &qh_object_handlers_intset;
	
	return retval;
}

static zend_object_value qh_object_new_intset(zend_class_entry *class_type TSRMLS_DC)
{
	return qh_object_new_intset_ex(class_type, NULL TSRMLS_CC);
}

static void qh_object_free_storage_intset(void *object TSRMLS_DC)
{
	php_qh_intset_obj *intern = (php_qh_intset_obj *) object;

	if (intern->hash) {
		qho *tmp_options = intern->hash->options;

		qhi_free(intern->hash);
		qho_free(tmp_options);
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);
}


static int process_flags(qho *options, long flags)
{
	options->check_for_dupes = (flags & QH_NO_DUPLICATES);
	if ((flags & QH_DO_NOT_USE_ZEND_ALLOC) == 0) {
		qh_set_memory_functions(options);
	}
	// if no hasher selected, pick jenkins1 as default
	if ((flags & QH_HASHER_MASK) == 0) {
		options->hasher = qha_jenkins1;
	} else {
		if (flags & QH_HASHER_NO_HASH) {
			options->hasher = qha_no_hash;
		} else if (flags & QH_HASHER_JENKINS1) {
			options->hasher = qha_jenkins1;
		} else if (flags & QH_HASHER_JENKINS2) {
			options->hasher = qha_jenkins2;
		}
	}
}

static int qh_intset_initialize(php_qh_intset_obj *obj, long size, long flags TSRMLS_DC)
{
	qho *options = qho_create();

	options->size = size;
	process_flags(options, flags);

	obj->hash = qhi_create(options);
	if (obj->hash == NULL) {
		qho_free(options);
		return 0;
	}
	return 1;
}

/* {{{ proto QuickHashIntSet QuickHashIntSet::__construct( int size, [ int options] )
   Creates a new QuickHashIntSet */
PHP_METHOD(QuickHashIntSet, __construct)
{
	long size;
	long options = 0;

	php_set_error_handling(EH_THROW, NULL TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &size, &options) == SUCCESS) {
		if (!qh_intset_initialize(zend_object_store_get_object(getThis() TSRMLS_CC), size, options TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not initialize set.");
		}
	}
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool QuickHashIntSet::add( int key )
   Adds an element with key key to the set */
PHP_METHOD(QuickHashIntSet, add)
{
	zval              *object;
	php_qh_intset_obj *intset_obj;
	long               key;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, qh_ce_intset, &key) == FAILURE) {
		RETURN_FALSE;
	}
	intset_obj = (php_qh_intset_obj *) zend_object_store_get_object(object TSRMLS_CC);
	RETURN_BOOL(qhi_set_add(intset_obj->hash, key));
}
/* }}} */

/* {{{ proto bool QuickHashIntSet::exists( int key )
   Tests whether the element with key key is part of the set */
PHP_METHOD(QuickHashIntSet, exists)
{
	zval              *object;
	php_qh_intset_obj *intset_obj;
	long               key;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, qh_ce_intset, &key) == FAILURE) {
		RETURN_FALSE;
	}
	intset_obj = (php_qh_intset_obj *) zend_object_store_get_object(object TSRMLS_CC);
	RETURN_BOOL(qhi_set_exists(intset_obj->hash, key));
}
/* }}} */

static uint32_t qh_intset_initialize_from_file(php_qh_intset_obj *obj, php_stream *stream, long flags TSRMLS_DC)
{
	php_stream_statbuf finfo;
	uint32_t           nr_of_elements, elements_read = 0;
	uint32_t           bytes_read;
	int32_t            key_buffer[1024];
	qho               *options = qho_create();

	// deal with options
	process_flags(options, flags);

	// obtain the filesize
	if (php_stream_stat(stream, &finfo) != 0) {
		qho_free(options);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not obtain file information");
		return 0;
	}

	// check whether we have a real file (and not a directory or something)
	if (!S_ISREG(finfo.sb.st_mode)) {
		qho_free(options);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "File is not a normal file");
		return 0;
	}

	// if the filesize is not an increment of 4, abort
	if (finfo.sb.st_size % 4 != 0) {
		qho_free(options);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "File is in the wrong format (not a multiple of 4 bytes)");
		return 0;
	}
	nr_of_elements = finfo.sb.st_size / 4;

	// override the nr of bucket lists as we know better
	options->size = nr_of_elements;

	// create the hash
	obj->hash = qhi_create(options);
	if (obj->hash == NULL) {
		qho_free(options);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't create set");
		return 0;
	}

	// read the elements and add them to the set
	do {
		bytes_read = php_stream_read(stream, &key_buffer, sizeof(key_buffer));
		qhi_set_add_elements_from_buffer(obj->hash, key_buffer, bytes_read / 4);
		elements_read += (bytes_read / 4);
	} while (elements_read < nr_of_elements);
	return nr_of_elements;
}

/* {{{ proto QuickHashIntSet QuickHashIntSet::loadFromFile( string filename [, int options ] )
   Creates a QuickHashIntSet from data in file filename */
PHP_METHOD(QuickHashIntSet, loadFromFile)
{
	char *filename;
	int   filename_len;
	long  options = 0;
	php_stream *stream;

	php_set_error_handling(EH_THROW, NULL TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &filename, &filename_len, &options) == FAILURE) {
		return;
	}

	qh_instantiate(qh_ce_intset, return_value TSRMLS_CC);
	stream = php_stream_open_wrapper(filename, "r", IGNORE_PATH | REPORT_ERRORS, NULL);
	if (stream) {
		uint32_t added_elements = qh_intset_initialize_from_file(zend_object_store_get_object(return_value TSRMLS_CC), stream, options);
		php_stream_close(stream);
	}
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

int qh_intset_save_to_file(php_stream *stream, php_qh_intset_obj *obj)
{
	uint32_t    idx;
	uint32_t    elements_in_buffer = 0;
	int32_t     key_buffer[1024];
	qhi        *hash = obj->hash;

	for (idx = 0; idx < hash->bucket_count; idx++)	{
		qhl *list = &(hash->bucket_list[idx]);
		qhb *p = list->head;
		qhb *n;

		if (p) {
			while(p) {
				n = p->next;

				key_buffer[elements_in_buffer] = p->key;
				elements_in_buffer++;

				if (elements_in_buffer == 1024) {
					if (php_stream_write(stream, key_buffer, elements_in_buffer * 4) != (elements_in_buffer * 4)) {
						return 0;
					}
					elements_in_buffer = 0;
				}

				p = n;
			}
		}
	}

	if (elements_in_buffer > 0) {
		if (php_stream_write(stream, key_buffer, elements_in_buffer * 4) != (elements_in_buffer * 4)) {
			return 0;
		}
	}
	return 1;
}

/* {{{ proto void QuickHashIntSet::saveToFile( string filename )
   Saves the hash to a file */
PHP_METHOD(QuickHashIntSet, saveToFile)
{
	char *filename;
	int   filename_len;
	zval              *object;
	php_qh_intset_obj *intset_obj;
	php_stream *stream;

	php_set_error_handling(EH_THROW, NULL TSRMLS_CC);
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, qh_ce_intset, &filename, &filename_len) == FAILURE) {
		return;
	}

	intset_obj = (php_qh_intset_obj *) zend_object_store_get_object(object TSRMLS_CC);
	stream = php_stream_open_wrapper(filename, "w", IGNORE_PATH | REPORT_ERRORS, NULL);

	if (stream) {
		qh_intset_save_to_file(stream, intset_obj);
		php_stream_close(stream);
	}
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

static uint32_t qh_intset_initialize_from_string(php_qh_intset_obj *obj, char *contents, long length, long flags TSRMLS_DC)
{
	uint32_t  nr_of_elements;
	qho      *options = qho_create();

	// deal with options
	process_flags(options, flags);

	// if the size is not an increment of 4, abort
	if (length % 4 != 0) {
		qho_free(options);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "String is in the wrong format (not a multiple of 4 bytes)");
		return 0;
	}
	nr_of_elements = length / 4;

	// override the nr of bucket lists as we know better
	options->size = nr_of_elements;

	// create the hash
	obj->hash = qhi_create(options);
	if (obj->hash == NULL) {
		qho_free(options);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't create set");
		return 0;
	}

	// read the elements and add them to the set
	qhi_set_add_elements_from_buffer(obj->hash, contents, nr_of_elements);
	return nr_of_elements;
}

/* {{{ proto QuickHashIntSet QuickHashIntSet::loadFromString( string contents [, int options ] )
   Creates a QuickHashIntSet from data in a string */
PHP_METHOD(QuickHashIntSet, loadFromString)
{
	char    *contents;
	int      contents_len;
	long     options = 0;
	uint32_t added_elements;

	php_set_error_handling(EH_THROW, NULL TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &contents, &contents_len, &options) == FAILURE) {
		return;
	}

	qh_instantiate(qh_ce_intset, return_value TSRMLS_CC);
	added_elements = qh_intset_initialize_from_string(zend_object_store_get_object(return_value TSRMLS_CC), contents, contents_len, options);
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */
