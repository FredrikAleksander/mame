// license:BSD-3-Clause
// copyright-holders:Felipe Sanches

#ifndef NLD_DM9314_H_
#define NLD_DM9314_H_

#include "netlist/nl_setup.h"

#define TTL_9314(name, cEQ, cMRQ, cS0Q, cS1Q, cS2Q, cS3Q, cD0, cD1, cD2, cD3)  \
		NET_REGISTER_DEV(TTL_9314, name)                                       \
		NET_CONNECT(name, GND, GND)                                            \
		NET_CONNECT(name, VCC, VCC)                                            \
		NET_CONNECT(name,  EQ,  cEQ)                                           \
		NET_CONNECT(name, MRQ, cMRQ)                                           \
		NET_CONNECT(name, S0Q, cS0Q)                                           \
		NET_CONNECT(name, S1Q, cS1Q)                                           \
		NET_CONNECT(name, S2Q, cS2Q)                                           \
		NET_CONNECT(name, S3Q, cS3Q)                                           \
		NET_CONNECT(name,  D0,  cD0)                                           \
		NET_CONNECT(name,  D1,  cD1)                                           \
		NET_CONNECT(name,  D2,  cD2)                                           \
		NET_CONNECT(name,  D3,  cD3)

#define TTL_9314_DIP(name)                                                     \
		NET_REGISTER_DEV(TTL_9314_DIP, name)

#endif /* NLD_DM9314_H_ */
