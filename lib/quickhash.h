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
#include <stdint.h>
#include <stdlib.h>

#ifndef QUICK_HASH_H
#define QUICK_HASH_H

/**
 * Hasher algorithm
 */
typedef uint32_t (*qha_t)(uint32_t key);

/**
 * Hash buckets
 */
typedef struct _qhb {
	uint32_t     key;
	struct _qhb *next;
	uint32_t     value_idx;
} qhb;

/**
 * List of hash buckets
 */
typedef struct _qhl {
	qhb      *head;
	qhb      *tail;
} qhl;

/**
 * Memory allocation functions struct
 */
typedef struct _qhm {
	void *(*malloc)(size_t size);
	void *(*calloc)(size_t nmemb, size_t size);
	void (*free)(void *ptr);
	void *(*realloc)(void *ptr, size_t size);
} qhm;

/**
 * Contains options for the hashes:
 * - size: the number of hash bucket lists to create
 * - check_for_dupes: whether insertions should be checked for duplicates
 */
typedef struct _qho {
	uint32_t size;
	char     check_for_dupes;
	qha_t    hasher;
	qhm      memory;
} qho;

/**
 * Integer hash type
 */
typedef struct _qhi {
	// hash related
	qha_t     hasher;
	qho      *options;

	// bucket lists
	uint32_t  bucket_count;
	qhl      *bucket_list;

	// for pre-allocating buckets
	int32_t   bucket_buffer_nr;
	uint32_t  bucket_buffer_pos;
	qhb     **bucket_buffer;

	// for values
	uint32_t  values_count;
	uint32_t  values_size;
	int32_t  *values;

	// statistics
	uint32_t  element_count;
#if DEBUG
	uint32_t  collisions;
#endif
} qhi;

/**
 * Function type to be used as an utility function with qhi_process_set
 */
typedef int (*qhi_buffer_apply_func)(void *context, int32_t *buffer, uint32_t elements);

uint32_t qha_jenkins1(uint32_t key);
uint32_t qha_jenkins2(uint32_t key);
uint32_t qha_no_hash(uint32_t key);


qho *qho_create(void);
void qho_free(qho *options);

uint32_t qhi_normalize_size(uint32_t size);
qhi *qhi_create(qho *options);
void qhi_free(qhi *hash);

/* sets */
int qhi_set_add(qhi *hash, int32_t position);
int qhi_set_exists(qhi *hash, int32_t position);
int qhi_set_delete(qhi *hash, int32_t position);

uint32_t qhi_set_add_elements_from_buffer(qhi *hash, int32_t *buffer, uint32_t nr_of_elements);
qhi *qhi_set_load_from_file(int fd, qho *options);
int qhi_set_save_to_file(int fd, qhi *hash);

/* hash */
int qhi_hash_add(qhi *hash, int32_t position, int32_t value);
int qhi_hash_get(qhi *hash, int32_t position, int32_t *value);
int qhi_hash_update(qhi *hash, int32_t position, int32_t value);
int qhi_hash_set(qhi *hash, int32_t position, int32_t value);

qhi *qhi_hash_load_from_file(int fd, qho *options);
int qhi_hash_save_to_file(int fd, qhi *hash);

#endif
