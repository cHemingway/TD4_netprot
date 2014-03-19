#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include "seatest.h"
#include "netprot_header.h"
#include "netprot_header_prv.h"

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

/* Test data size is stored correctly */
void test_size(void) {
	struct netstruct header;
	char *ptr; /* So we can index into header */
	int err;
	int sizein = 0xaabbccdd; /* Designed to trip up byte order */
	int sizeout;

	err = netprot_header_append(&header, 0, sizein, 0, 0);
	assert_false(err);

	/* Check size is valid */
	ptr = getoffsetptr(header, 24);  /* Size is 24 bytes in */
	sizeout = extract_uint32(ptr);
	assert_int_equal(sizein, sizeout);
}

/* Test count is stored correctly */
void test_count(void) {
	struct netstruct header;
	char *ptr; /* So we can index into header */
	int err;
	int countin = 0xbbaaddff; /* Designed to trip up byte order */
	int countout;

	err = netprot_header_append(&header, countin, 0, 0, 0);
	assert_false(err);

	/* Check size is valid */
	ptr = getoffsetptr(header, 4);  /* Count is 4 bytes in */
	countout = extract_uint32(ptr);
	assert_int_equal(countin, countout);
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
	run_test(test_size);
	run_test(test_count);
	test_fixture_end();
}

/* Test for pointers being null */
void test_decode_nullptr(void) {
	/* Params */
	void *decode;
	int decodelen;
	char *dataout; 
	int datasize;
	int count; 
	long long dt_ns; 
	char flags;
	/* Return value */
	int err;

	/* Test all values null */
	err = netprot_header_decode(NULL, 0, NULL, NULL, NULL, NULL, NULL);
	assert_int_equal(NP_ERR_PARAM, err);

	/* Test decode null */
	err = netprot_header_decode(NULL, decodelen, &dataout, &datasize, &count, &dt_ns, &flags);
	assert_int_equal(NP_ERR_PARAM, err);

	/* Test dataout null */
	err = netprot_header_decode(decode, decodelen, NULL, &datasize, &count, &dt_ns, &flags);
	assert_int_equal(NP_ERR_PARAM, err);

	/* Test datasize null */
	err = netprot_header_decode(decode, decodelen, &dataout, NULL, &count, &dt_ns, &flags);
	assert_int_equal(NP_ERR_PARAM, err);

	/* Test count null */
	err = netprot_header_decode(decode, decodelen, &dataout, &datasize, NULL, &dt_ns, &flags);
	assert_int_equal(NP_ERR_PARAM, err);

	/* Test dt_ns null */
	err = netprot_header_decode(decode, decodelen, &dataout, &datasize, &count, NULL, &flags);
	assert_int_equal(NP_ERR_PARAM, err);

	/* Test flags null */
	err = netprot_header_decode(decode, decodelen, &dataout, &datasize, &count, &dt_ns, NULL);
	assert_int_equal(NP_ERR_PARAM, err);
}

/* Test to see if data can be encoded and decoded */
void test_decode_data(void) {
	struct netstruct *header, *headercopy;
	char datain[] = "Example Data, reasonably long to catch errors";
	size_t datainsize = sizeof(datain) / sizeof(datain[0]);
	size_t struct_size;

	/* Params */
	int decodelen;
	char *dataout; 
	int dataoutsize;
	int count; 
	long long dt_ns; 
	char flags;
	int err;

	/* Check data size is valid */
	assert_int_equal(datainsize, strlen(datain)+1);

	/* Malloc space for header */
	header = (struct netstruct *)
		malloc(sizeof(struct netstruct) + datainsize);

	/* Stick the data in the header */
	memcpy(header->data, datain, datainsize);

	/* Now append the header*/
	err = netprot_header_append(header, 0, datainsize, 0, 0);
	assert_false(err);

	/* Get the size */
	struct_size = netprot_header_getsize(header);

	/* Create a new struct and copy to that */
	headercopy = (struct netstruct *) malloc(struct_size);

	memcpy(headercopy, header, struct_size);

	/* Extract the data */
	err = netprot_header_decode(headercopy, struct_size, &dataout, &dataoutsize, &count, &dt_ns, &flags);
	assert_int_equal(NP_ERR_OK, err);

	/* Compare datasize */
	assert_int_equal(datainsize, dataoutsize);

	/* Compare data*/
	assert_n_array_equal(datain, headercopy->data, datainsize);
}

void test_netprot_header_decode(void) {
	test_fixture_start();
	run_test(test_decode_nullptr);
	run_test(test_decode_data);
	test_fixture_end();
}

void all_tests(void) {
	test_netprot_header_append();
	test_netprot_header_decode();
}

/* Actually run the tests */
int main(int argc, char const *argv[])
{
	/* Invert for make */
	return !run_tests(all_tests);
}