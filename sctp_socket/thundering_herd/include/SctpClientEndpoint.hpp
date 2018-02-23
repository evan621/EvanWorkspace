#include "SctpEndpoint.hpp"

#include "SctpNotification.hpp"
#include "IoMultiplex.hpp"
#include <string>
#include <stdint.h>
#include <memory>
#include <thread>
#include "SctpMessage.hpp"
#include "SctpSocketOperation.hpp"


class SctpClientEndpoint: public SctpEndpoint
{
public:
	SctpClientEndpoint(IoMultiplex& multiRecv);
	~SctpClientEndpoint();

	void SendMsg();
	
private:
    int onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg);
	int onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg);
	int SctpMsgHandler(int sock_fd);

	void ReadUserCmd(int fd);

	std::unique_ptr<SctpSocketOperation> sock_op;
	IoMultiplex& io_multi;
	SctpNotification notification;
	
	bool continuepoll;
};