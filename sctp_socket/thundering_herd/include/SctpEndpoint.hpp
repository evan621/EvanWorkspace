#ifndef _SCTP_ENDPOINT
#define _SCTP_ENDPOINT
#include <vector>
class SctpEndpoint
{
public:
	virtual ~SctpEndpoint() = default;
	virtual void SendMsg(std::vector<char> msg) = 0;
};

#endif