#include "netprot_header.h"
#include "netprot_header_prv.h"

#include <stdint.h>
#include <stddef.h>

/* Include correct hton, ntohl */
#ifdef FNET
#include "fnet.h"
#else
#include <arpa/inet.h>
#endif

int netprot_header_append(struct netstruct *toappend, int count,
 						  int datasize, long long dt_ns, char flags) 
{
	int dt_s;
	/* Check Pointers */
	if (toappend==NULL) {
		return -1;
	}

	/* Set default fields */
	toappend->version 	 = NETPROT_VERSION;
	toappend->header_size = offsetof(struct netstruct, data);
	toappend->channel_no = 0; /* Hardcoded for now */
	toappend->ts_s		 = 0; /* Timestamp is not used yet */
	toappend->ts_ns		 = 0;  

	/* Set flags */
	flags |= TS_INVALID; /* No timstamp support yet */
	toappend->flags = (uint8_t)(flags & 0xff);

	/* Set count */
	toappend->count = htonl(count);

	/* Calculate and set dt */
	dt_s = dt_ns / 1000000000u;
	toappend->dt_s = htonl(dt_s);
	toappend->dt_ns = htonl(dt_ns % 1000000000u);

	/* Set length */
	toappend->size = htonl(datasize);

	return 0; //Success
}