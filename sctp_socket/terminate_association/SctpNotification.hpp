#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

class SctpNotification{
public:
	void Print(char* notify_buf);
	SctpNotification(){}
	~SctpNotification(){}
	
private:
	void PrintAssocChange(union sctp_notification *notification);
	void PrintPeerAddrChange(union sctp_notification *notification);
	void PrintRemoteError(union sctp_notification *notification);
	void PrintSendFailed(union sctp_notification *notification);
	void PrintAdaptionIndication(union sctp_notification *notification);
	void PrintPartialDeliveryEvent(union sctp_notification *notification);
	void PrintShutDownEvent(union sctp_notification *notification);
	void PrintAuthenticationIndication(union sctp_notification *notification);
	void PrintSenderDryEvent(union sctp_notification *notification);
};

