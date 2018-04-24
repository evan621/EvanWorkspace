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
	SctpClientEndpoint();
	~SctpClientEndpoint();
	
	void Start() override;

private:
    int onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg);
	int onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg);
	int SctpMsgHandler(int sock_fd);
	void SendMsg();

	void ReadUserCmd(int fd);

	std::unique_ptr<SctpSocketOperation> sock_op;
	std::unique_ptr<IoMultiplex> io_multi;
	SctpNotification notification;
	
	bool continuepoll;
};