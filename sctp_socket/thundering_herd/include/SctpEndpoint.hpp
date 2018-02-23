#ifndef _SCTP_ENDPOINT
#define _SCTP_ENDPOINT
class SctpEndpoint
{
public:
	virtual ~SctpEndpoint() = default;
	virtual void SendMsg() = 0;
};

#endif