#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "quickhash.h"

/**
 * Hashes the key
 *
 * The algorithm is from: http://burtleburtle.net/bob/hash/integer.html
 *
 * Parameters:
 * - key, the key to be hashsed
 *
 * Returns:
 * - the hash key
 */
uint32_t qha_jenkins1(uint32_t key)
{
	key = (key ^ 61) ^ (key >> 16);
	key = key + (key << 3);
	key = key ^ (key >> 4);
	key = key * 0x27d4eb2d;
	key = key ^ (key >> 15);
	return key;
}

/**
 * Hashes the key
 *
 * The algorithm is from: http://www.concentric.net/~Ttwang/tech/inthash.htm
 *
 * Parameters:
 * - key, the key to be hashsed
 *
 * Returns:
 * - the hash key
 */
uint32_t qha_jenkins2(uint32_t key)
{
	key = (key+0x7ed55d16) + (key<<12);
	key = (key^0xc761c23c) ^ (key>>19);
	key = (key+0x165667b1) + (key<<5);
	key = (key+0xd3a2646c) ^ (key<<9);
	key = (key+0xfd7046c5) + (key<<3);
	key = (key^0xb55a4f09) ^ (key>>16);
	return key;
}

/**
 * 'Hashes' the key by passing it straight through
 *
 * Parameters:
 * - key, the key to be hashsed
 *
 * Returns:
 * - the hash key
 */
uint32_t qha_no_hash(uint32_t key)
{
	return key;
}


/**
 * Defines the number of buckets to pre-allocate
 */
#define QHB_BUFFER_PREALLOC_INC 1024

/**
 * Allocates a hash bucket.
 *
 * The algorithm allocates memory for QHB_BUFFER_PREALLOC_INC buckets at the
 * same time to avoid having too many allocs and frees.
 *
 * Returns:
 * - A newly allocated hash bucket or NULL upon allocation failure
 */
inline qhb *qhb_create(qhi *hash)
{
	qhb *tmp = NULL;

	if (hash->bucket_buffer_pos % QHB_BUFFER_PREALLOC_INC == 0) {
		hash->bucket_buffer_nr++;
		hash->bucket_buffer = hash->options->memory.realloc(hash->bucket_buffer, sizeof(qhb*) * (hash->bucket_buffer_nr + 1));
		if (!hash->bucket_buffer) {
			return NULL;
		}
		hash->bucket_buffer[hash->bucket_buffer_nr] = hash->options->memory.malloc(sizeof(qhb) * QHB_BUFFER_PREALLOC_INC);
		if (!hash->bucket_buffer[hash->bucket_buffer_nr]) {
			return NULL;
		}
		hash->bucket_buffer_pos = 0;
	}
	tmp = &(hash->bucket_buffer[hash->bucket_buffer_nr][hash->bucket_buffer_pos]);
	hash->bucket_buffer_pos++;

	return tmp;
}

/**
 * Creates a new options struct and sets defaults
 *
 * Returns:
 * - An options struct with default values.
 */
qho *qho_create(void)
{
	qho *tmp = malloc(sizeof(qho));

	tmp->size = 1024;
	tmp->check_for_dupes = 0;
	tmp->memory.malloc = malloc;
	tmp->memory.calloc = calloc;
	tmp->memory.realloc = realloc;
	tmp->memory.free = free;
	tmp->hasher = qha_jenkins2;

	return tmp;
}

/**
 * Frees an options struct
 *
 * Parameters:
 * - options: the options struct to free
 */
void qho_free(qho *options)
{
	free(options);
}

/**
 * Normalizes the size to a power of two.
 *
 * The function also limits the size from sizes 4 to 4194304.
 *
 * Parameters:
 * - size: the number of (expected) elements in the set.
 *
 * Returns:
 * - The normalized size
 */
uint32_t qhi_normalize_size(uint32_t size)
{
	uint32_t i;

	if (size > 1<<22) {
		return 1<<22;
	}
	for (i = 22; i > 2; i--) {
		if (size > (1<< (i - 1))) {
			return 1<<i;
		}
	}
	return 4;
}

/**
 * Creates a new integer quick hash
 *
 * Parameters:
 * - options: the options to create the hash with. This structure contains at
 *   least the nr of hash buckets, and whether set additions should be checked
 *   for duplicates. See the description of qho for a full list of options.
 *   This options struct must *NOT* be freed before the hash itself, as it
 *   contains pointers to memory allocation functions.
 *
 * Returns:
 * - a pointer to the hash, or NULL if:
 *   - upon memory allocation failures.
 *   - size < 4
 */
qhi *qhi_create(qho *options)
{
	qhi      *tmp;
	uint32_t  size;

	if (options == NULL) {
		return NULL;
	}

	size = qhi_normalize_size(options->size);

	tmp = options->memory.malloc(sizeof(qhi));
	if (!tmp) {
		return NULL;
	}

	tmp->hasher = options->hasher;
	tmp->bucket_count = size;

	tmp->bucket_buffer_nr  = -1;
	tmp->bucket_buffer_pos = 0;
	tmp->bucket_buffer     = NULL;

	tmp->bucket_list = options->memory.calloc(sizeof(qhl) * size, 1);
	if (!tmp->bucket_list) {
		options->memory.free(tmp);
		return NULL;
	}

	tmp->options = options;

#if DEBUG
	tmp->collisions = 0;
#endif

	return tmp;
}

/**
 * Frees a quickhash
 *
 * Parameters:
 * - hash: A valid quickhash
 */
void qhi_free(qhi *hash)
{
	int32_t idx;

#if DEBUG
	printf("Collisions: %u\n", hash->collisions);
#endif
	for (idx = 0; idx <= hash->bucket_buffer_nr; idx++) {
		hash->options->memory.free(hash->bucket_buffer[idx]);
	}

	if (hash->bucket_buffer) {
		hash->options->memory.free(hash->bucket_buffer);
	}
	hash->options->memory.free(hash->bucket_list);
	hash->options->memory.free(hash);
}

/**
 * Obtains the hash value for the specified key
 *
 * Parameters:
 * - hash: A valid quickhash
 * - key: The key
 *
 * Returns:
 * - The hashed key
 */
inline uint32_t qhi_set_hash(qhi *hash, uint32_t key)
{
	uint32_t idx = hash->hasher(key);
	return idx & (hash->bucket_count - 1);
}

/**
 * Internal: checks whether a key exists in a specific list
 *
 * Parameters:
 * - list: the list belonging to the index of the hashed key
 * - key: the element's key
 *
 * Returns:
 * - 1 if the key exists in the list, 0 if not
 */
static int find_entry_in_list(qhl *list, int32_t key)
{
	if (!list->head) {
		// there is no bucket list for this hashed key
		return 0;
	} else {
		qhb *p = list->head;

		// loop over the elements in this bucket list to see if the key exists
		do {
			if (p->key == key) {
				return 1;
			}
			p = p->next;
		} while(p);
	}
	return 0;
}

/**
 * Adds a new element to the hash
 *
 * Parameters:
 * - hash: A valid quickhash
 * - key: The key
 *
 * Returns:
 * - 1 if the element was added or 0 if the element couldn't be added
 */
int qhi_set_add(qhi *hash, int32_t key)
{
	uint32_t idx;
	qhl     *list;
	qhb     *bucket;

	// obtain the hashed key, and the bucket list for the hashed key
	idx = qhi_set_hash(hash, key);
	list = &(hash->bucket_list[idx]);

	// check if we already have the key in the list if requested
	if (hash->options->check_for_dupes && find_entry_in_list(list, key)) {
		return 0;
	}

	// create new bucket
	bucket = qhb_create(hash);
	if (!bucket) {
		return 0;
	}
	bucket->key = key;
	bucket->next = NULL;

	// add bucket to list
	if (list->head == NULL) {
		// first bucket in list
		list->head = bucket;
		list->tail = bucket;
	} else {
		// following bucked in a list
		list->tail->next = bucket;
		list->tail = bucket;
#if DEBUG
		hash->collisions++;
#endif
	}
	return 1;
}

/**
 * Tests whether the key exists in the set
 *
 * Parameters:
 * - hash: A valid quickhash
 * - key: The key
 *
 * Returns:
 * - 1 if the element is part of the set or 0 if the element is not part of the
 *   set
 */
int qhi_set_exists(qhi *hash, int32_t key)
{
	uint32_t idx;
	qhl     *list;

	// obtain the hashed key, and the bucket list for the hashed key
	idx = qhi_set_hash(hash, key);
	list = &(hash->bucket_list[idx]);

	return find_entry_in_list(list, key);
}

/**
 * Adds elements contained in a buffer to set
 *
 * Parameters:
 * - hash: the set to add elements to
 * - buffer: the buffer containing the keys
 * - nr_of_elements: the number of elements in the buffer
 *
 * Returns:
 * - The number of elements that were added to the set. This number can be less
 *   than the number of elements in the buffer because de-duping might happen.
 */
uint32_t qhi_set_add_elements_from_buffer(qhi *hash, int32_t *buffer, uint32_t nr_of_elements)
{
	uint32_t i;
	uint32_t added = 0;

	for (i = 0; i < nr_of_elements; i++) {
		added += qhi_set_add(hash, buffer[i]);
	}
	return added;
}

/**
 * Loads a set from a file pointed to by the file descriptor
 *
 * Parameters:
 * - fd: a file descriptor that is suitable for reading from
 * - options: the options to create the hash with. This structure contains at
 *   least the nr of hash buckets, and whether set additions should be checked
 *   for duplicates. See the description of qho for a full list of options.
 *
 * Returns:
 * - A new hash, or NULL upon failure
 */
qhi *qhi_set_load_from_file(int fd, qho *options)
{
	struct stat finfo;
	uint32_t    nr_of_elements, elements_read = 0;
	uint32_t    bytes_read;
	int32_t     key_buffer[1024];
	qhi        *tmp;

	if (fstat(fd, &finfo) != 0) {
		return NULL;
	}

	// if the filesize is not an increment of 4, abort
	if (finfo.st_size % 4 != 0) {
		return NULL;
	}
	nr_of_elements = finfo.st_size / 4;

	// override the nr of bucket lists as we know better
	options->size = qhi_normalize_size(nr_of_elements);
#if DEBUG
	printf("Picking size: %u\n", options->size);
#endif

	// create the set
	tmp = qhi_create(options);
	if (!tmp) {
		return NULL;
	}

	// read the elements and add them to the set
	do {
		bytes_read = read(fd, &key_buffer, sizeof(key_buffer));
		qhi_set_add_elements_from_buffer(tmp, key_buffer, bytes_read / 4);
		elements_read += (bytes_read / 4);
	} while (elements_read < nr_of_elements);
	return tmp;
}

/**
 * Saves a set to a file point to by the file descriptor
 *
 * Parameters:
 * - hash: the set to write
 * - fd: a file descriptor that is suitable for reading to
 *
 * Returns:
 * - 1 on success, and 0 on failure
 */
int qhi_set_save_to_file(int fd, qhi *hash)
{
	uint32_t    idx;
	uint32_t    elements_in_buffer = 0;
	int32_t     key_buffer[1024];

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
					if (write(fd, key_buffer, elements_in_buffer * 4) != (elements_in_buffer * 4)) {
						return 0;
					}
					elements_in_buffer = 0;
				}

				p = n;
			}
		}
	}

	if (elements_in_buffer > 0) {
		if (write(fd, key_buffer, elements_in_buffer * 4) != (elements_in_buffer * 4)) {
			return 0;
		}
	}
	return 1;
}
