
#ifndef __NETPROT_HEADER_H__
#define __NETPROT_HEADER_H__

#include <stdint.h>

#ifndef NETPROT_VERSION
#define NETPROT_VERSION 1
#endif

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




#endif /* __NETPROT_HEADER_H__ */