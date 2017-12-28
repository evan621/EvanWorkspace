#include "SctpEndpoint.hpp"

#include "SctpNotification.hpp"
#include <string>
#include <stdint.h>
#include <memory>
#include <thread>
#include "SctpMessage.hpp"
#include "SctpSocketOperation.hpp"


class SctpClientEndpoint: public SctpEndpoint
{
public:
	SctpClientEndpoint();
	~SctpClientEndpoint();
	
	void SendMsg() override;

private:
    int onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg);
	int onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg);
	void ThreadHandler();

	std::unique_ptr<SctpSocketOperation> sock_op;
	SctpNotification notification;
	std::thread pollThread;
	
	bool continuepoll;
};