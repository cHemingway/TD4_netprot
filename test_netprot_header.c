#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include "seatest.h"
#include "netprot_header.h"


#define getoffsetptr(val, offset) (((char*)&(val)) + (offset))
#define extract_uint8(ptr)  ((char) (*(uint8_t *)(ptr)) & 0xff)
#define extract_uint32(ptr) (ntohl((int)*((uint32_t *)(ptr))) & 0xffffffff)

/* Test to see if it rejects a null pointer */
void test_pack_nullptr(void) {
	struct netstruct *data = NULL; /* Invalid data */

	/* Should return -1 */
	assert_int_equal(
		netprot_header_append(data, 0, 0, 0, 0),
		-1 );
}

/* Test to see if very basic pack will succeed */
void test_pack(void) {
	struct netstruct *header = NULL;
	int data_len = 1000;
	int err;

	/* Allocate the header */
	header = malloc(sizeof(struct netstruct) + data_len);

	/* Try and use it */
	err = netprot_header_append(header, 0, data_len, 10000, 0);
	assert_false(err);
}

/* Test for flags variable */
void test_flags(void) {
	struct netstruct header; /* No data */
	int err;

	/* Check data invalid flag */
	err = netprot_header_append(&header, 0, 0, 0, DATA_INVALID);
	assert_false(err);
	assert_bit_set(1, header.flags);

	/* Check TS invalid is default set */
	err = netprot_header_append(&header, 0, 0, 0, 0);
	assert_false(err);
	assert_bit_mask_matches(header.flags, 0x02);
}

/* Test for version */
void test_version(void) {
	struct netstruct header;
	uint8_t *versionptr;
	int version;
	int err;

	err = netprot_header_append(&header, 0, 0, 0, 0);
	assert_false(err);

	/* Cast first byte to uint8_t to check */
	versionptr = (uint8_t*)(&header);
	version = *versionptr;
	assert_int_equal(1, version);
}

/* Test to see if dt is valid */
void test_dt(void) {
	struct netstruct header;
	char *ptr; /* So we can index into header */
	int dt_s, dt_ns;
	long long dt = 2713195743; /* 2.713..  */
	int err;

	err = netprot_header_append(&header, 0, 0, dt, 0);
	assert_false(err);

	/* Test seconds */
	ptr = getoffsetptr(header, 16); /* 16 bytes in */
	dt_s = extract_uint32(ptr);
	assert_int_equal(dt/1000000000, dt_s);

	/* Test nanoseconds */
	ptr = getoffsetptr(header, 20); /* 16 bytes in */
	dt_ns = extract_uint32(ptr);
	assert_int_equal(dt%1000000000, dt_ns);
}

/* Test to see if header_size is valid */
//FIXME: Does not pass currently, find out why!
void test_header_size(void) {
	struct netstruct *header;
	char *header_size_ptr, *data_ptr;
	int header_size;
	char data[] = "Example Data, reasonably long to catch errors";
	size_t datasize = sizeof(data);
	int err;

	/* Check data size is valid */
	assert_int_equal(datasize, strlen(data)+1);

	/* Malloc space for header */
	header = (struct netstruct *)
		malloc(sizeof(struct netstruct) + datasize);

	/* Stick the data in the header */
	memcpy(header->data, data, datasize);

	/* Now append the header*/
	err = netprot_header_append(header, 0, datasize, 0, 0);
	assert_false(err);

	/* Get the header size */
	header_size_ptr = getoffsetptr(*header, 1);
	header_size = extract_uint8(header_size_ptr);

	/* Check data is valid */
	data_ptr = getoffsetptr(*header, header_size);
	err = strncmp(data_ptr, data, datasize);
	assert_int_equal(0, err);
}

/* Tests for netprot_header_append*/
void test_netprot_header_append(void) {
	test_fixture_start();
	run_test(test_pack_nullptr);
	run_test(test_pack);
	run_test(test_flags);
	run_test(test_version);
	run_test(test_dt);
	run_test(test_header_size);
	test_fixture_end();
}


void all_tests(void) {
	test_netprot_header_append();
}

/* Actually run the tests */
int main(int argc, char const *argv[])
{
	/* Invert for make */
	return !run_tests(all_tests);
}