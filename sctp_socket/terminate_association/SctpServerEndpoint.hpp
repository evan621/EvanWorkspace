#include "SctpEndpoint.hpp"
#include "SctpNotification.hpp"
#include <string>
#include <stdint.h>
#include <thread>

class SctpServerEndpoint: public SctpEndpoint
{
public:
	SctpServerEndpoint(std::string localIp, std::uint32_t port);
	~SctpServerEndpoint();
	
	void SendMsg() override;
	void RegisterMsgHandler() override;
	int StartPoolForMsg() override;

private:
	void SetSocketOpt();
	void CreateSocket();
	void Bind(std::string localIp, uint32_t port);
	int SctpMsgHandler(int sock_fd);

	int sock_fd{-1};
	std::thread pollThread;
	SctpNotification notification;
};