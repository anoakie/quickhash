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
	int  fd;
	
	options->size = 2000;
	options->check_for_dupes = 1;
	options->key_type = QHI_KEY_TYPE_STRING;
	h = qhi_create(options);

	qhi_hash_add(h, (qhv) "één",  (qhv) 1);
	qhi_hash_add(h, (qhv) "twee", (qhv) 2);
	qhi_hash_add(h, (qhv) "drie", (qhv) 3);
	qhi_hash_add(h, (qhv) "vier", (qhv) 4);

	fd = open("/tmp/test-save", O_RDWR | O_TRUNC | O_CREAT, 0666);
	qhi_hash_save_to_file(fd, h);
	close(fd);

	qhi_free(h);
	qho_free(options);

	return 0;
}
