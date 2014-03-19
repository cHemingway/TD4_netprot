
#ifndef __NETPROT_HEADER_H__
#define __NETPROT_HEADER_H__

#include <stdint.h>

#ifndef NETPROT_VERSION
#define NETPROT_VERSION 1
#endif

enum netprotheader_err {
	NP_ERR_CORRUPT = -2, 	/* If recieved data is corrupt  */
	NP_ERR_PARAM  = -1, 	/* If a parameter is invalid (e.g. NULL ptr) */
	NP_ERR_OK	   = 0 		/* No error */
};
enum netprot_flags {
	DATA_INVALID = 0x01,
	TS_INVALID   = 0x02
};

struct __attribute__ ((__packed__)) netstruct {
	uint8_t 	version;
	uint8_t 	header_size;
	uint8_t 	flags;
	uint8_t 	channel_no;
	uint32_t 	count;
	uint32_t 	ts_s;
	uint32_t 	ts_ns;
	uint32_t 	dt_s;
	uint32_t 	dt_ns;
	uint32_t 	size;
	char	 	data[0];
};



int netprot_header_append(struct netstruct *toappend, int count,
 						  int datasize, long long dt_ns, char flags);


int netprot_header_decode(const void *decode, int decodelen,
							char **dataout, int *datasize,
							int *count, long long *dt_ns, char *flags);

int netprot_header_getsize(struct netstruct *in);

#endif /* __NETPROT_HEADER_H__ */