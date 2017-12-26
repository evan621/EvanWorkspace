#include "SctpEndpoint.hpp"

#include "SctpNotification.hpp"
#include <string>
#include <stdint.h>

class SctpClientEndpoint: public SctpEndpoint
{
public:
	SctpClientEndpoint();
	~SctpClientEndpoint();
	
	void SendMsg() override;
	void RegisterMsgHandler() override;
	int StartPoolForMsg(int timeout);

private:
	void SetSocketOpt();
	void CreateSocket();
	int SctpMsgHandler(int sock_fd);
	void SendAbort();

	int sock_fd{-1};
	SctpNotification notification;
};