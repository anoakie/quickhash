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
#include "quickhash.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	qhi *h;
	qho *options = qho_create();
	int32_t value = 0;
	int32_t i;
	
	options->size = 4;
	options->check_for_dupes = 1;
	h = qhi_create(options);

	for (i = 0; i < 2048; i++) {
		qhi_set_add(h, i);
	}

	// delete first in list
	qhi_set_delete(h, 6);
	for (i = 0; i < 2048; i++) {
		value += qhi_set_exists(h, i);
	}
	printf("total: %d\n", value);
	value = 0;

	// delete middle
	qhi_set_delete(h, 123);
	for (i = 0; i < 2048; i++) {
		value += qhi_set_exists(h, i);
	}
	printf("total: %d\n", value);
	value = 0;

	// delete last of list
	qhi_set_delete(h, 2015);
	for (i = 0; i < 2048; i++) {
		value += qhi_set_exists(h, i);
	}
	printf("total: %d\n", value);
	value = 0;

	qhi_free(h);
	qho_free(options);
	return 0;
}
