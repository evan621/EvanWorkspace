#ifndef _SCTP_ENDPOINT
#define _SCTP_ENDPOINT
class SctpEndpoint
{
public:
	virtual ~SctpEndpoint() = default;
	virtual void Start() = 0;
};

#endif