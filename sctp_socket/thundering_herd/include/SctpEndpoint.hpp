#ifndef _SCTP_ENDPOINT
#define _SCTP_ENDPOINT
#include <vector>
#include "common.hpp"

class SctpEndpoint
{
public:
	virtual ~SctpEndpoint() = default;
	virtual void SendMsg(std::vector<char> msg) = 0;
    virtual void register_newconn_handler(newconn_handler_callbk cb) = 0;
};

#endif
