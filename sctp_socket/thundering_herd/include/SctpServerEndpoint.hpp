#include "SctpEndpoint.hpp"
#include "SctpNotification.hpp"
#include "SctpSocketOperation.hpp"
#include "IoMultiplex.hpp"
#include <string>
#include <stdint.h>
#include <thread>
#include <memory>
#include "SctpMessage.hpp"
#include <unistd.h>

struct AssociationInfo{
	std::shared_ptr<std::string> ip;
	unsigned int port;
	unsigned int stream;
};

class SctpServerEndpoint: public SctpEndpoint
{
public:
	SctpServerEndpoint(std::string localIp, std::uint32_t port, IoMultiplex& multiRecv);
	~SctpServerEndpoint();
	void SendMsg();

private:
    int onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg);
	int onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg);
	void ReadUserCmd(int fd);
	
	int SctpMsgHandler(int sock_fd);

	std::unique_ptr<SctpSocketOperation> sock_op;
	IoMultiplex& io_multi;
	
	//std::map<unsigned int, AssociationInfo> association_list;
	std::unique_ptr<AssociationInfo> assoInfo;
	
	SctpNotification notification;
};