#include "SctpNotification.hpp"
#include <string>
#include <iostream>

#define INET6_ADDRSTRLEN 46

void SctpNotification::PrintAssocChange(union sctp_notification *notification)
{
	sctp_assoc_change *sctpAssociationChange;
	std::string str;
	
	sctpAssociationChange = &notification->sn_assoc_change;
	switch(sctpAssociationChange->sac_state)
	{
		case SCTP_COMM_UP:
			str = "COMMUNICATION UP";
			break;
		case SCTP_COMM_LOST:
			str = "COMMUNICATION LOST";
			break;
		case SCTP_RESTART:
			str = "SCTP RESTART";
			break;
		case SCTP_SHUTDOWN_COMP:
			str = "SHUTDOWN COMPLETE";
			break;
		case SCTP_CANT_STR_ASSOC:
			str = "CAN'T START ASSOCIATION";
			break;
		default:
			str = "UNKOWN";
			break;
	}
	
	std::cout << "[Notification]: SCTP_ASSOC_CHANGE: " << str << " received with assoc id = "<< sctpAssociationChange->sac_assoc_id << std::endl;
}

void SctpNotification::PrintPeerAddrChange(union sctp_notification *notification)
{
	std::string 	str;
	sctp_paddr_change *sctpPeerAddChange;
	char            addrbuf[INET6_ADDRSTRLEN];
	const char     *ap;

	sctpPeerAddChange = &notification->sn_paddr_change;
	
	switch(sctpPeerAddChange->spc_state) {
		case SCTP_ADDR_AVAILABLE:
			str = "ADDRESS AVAILABLE";
			break;
		case SCTP_ADDR_UNREACHABLE:
			str = "ADDRESS UNREACHABLE";
			break;
		case SCTP_ADDR_REMOVED:
			str = "ADDRESS REMOVED";
			break;
		case SCTP_ADDR_ADDED:
			str = "ADDRESS ADDED";
			break;
		case SCTP_ADDR_MADE_PRIM:
			str = "ADDRESS PRIMARY";
			break;
		case SCTP_ADDR_CONFIRMED:
			str = "ADDRESS CONFIRMED";
			break;
		default:
			str = "UNKNOWN";
			std::cout << "[Notification]: UNKNOWN state = " << sctpPeerAddChange->spc_state << std::endl;
			break;
	}

	// try to get the IP address 	
	if (sctpPeerAddChange->spc_aaddr.ss_family == AF_INET) {
		sockaddr_in* sin = (struct sockaddr_in*) &sctpPeerAddChange->spc_aaddr;
		ap = inet_ntop(AF_INET, &sin->sin_addr, addrbuf, INET6_ADDRSTRLEN);
	} else {
		sockaddr_in6* sin6 = (struct sockaddr_in6 *) &sctpPeerAddChange->spc_aaddr;
		ap = inet_ntop(AF_INET6, &sin6->sin6_addr, addrbuf, INET6_ADDRSTRLEN);
	}
	std::cout << "[Notification]: ip = " << addrbuf << std::endl;
	std::cout << "[Notification]: SCTP_PEER_ADDR_CHANGE:  "<< str << " received with assoc id = " << sctpPeerAddChange->spc_assoc_id << std::endl;
}

void SctpNotification::PrintRemoteError(union sctp_notification *notification)
{
	sctp_remote_error *sctpRemoteErr;
	sctpRemoteErr = &notification->sn_remote_error;
	std::cout << "[Notification]: SCTP_REMOTE_ERROR received with assoc id = " << sctpRemoteErr->sre_assoc_id << " ,error= " << sctpRemoteErr->sre_error << std::endl;
}

void SctpNotification::PrintSendFailed(union sctp_notification *notification)
{
	sctp_send_failed *sctpSendFailed;
	sctpSendFailed = &notification->sn_send_failed;
	std::cout << "[Notification]: SCTP_SEND_FAILED received with assoc id = " << sctpSendFailed->ssf_assoc_id << " ,error = " << sctpSendFailed->ssf_error << std::endl;
}

void SctpNotification::PrintAdaptionIndication(union sctp_notification *notification)
{
	sctp_adaptation_event *sctpAdaptEvent;
	sctpAdaptEvent = &notification->sn_adaptation_event;
	std::cout << "[Notification]: SCTP_ADAPTION_INDICATION: " << sctpAdaptEvent->sai_adaptation_ind << std::endl;
}

void SctpNotification::PrintPartialDeliveryEvent(union sctp_notification *notification)
{
	sctp_pdapi_event *sctpPdapiEvent;
	sctpPdapiEvent = &notification->sn_pdapi_event;
	
	if(sctpPdapiEvent->pdapi_indication == SCTP_PARTIAL_DELIVERY_ABORTED)
		std::cout << "[Notification]: SCPT_PARTIAL_DELIVERY_ABORTED!" << std::endl;
	else
		std::cout << "[Notification]: Unkown SCPT_PARTIAL_DELIVERY_EVENT!" << std::endl;

}

void SctpNotification::PrintShutDownEvent(union sctp_notification *notification)
{
	sctp_shutdown_event *sctpShutdownEvent;
	sctpShutdownEvent = &notification->sn_shutdown_event;
	
	std::cout << "[Notification]: SCTP_SHUTDOWN_EVENT: assoc id = " << sctpShutdownEvent->sse_assoc_id << std::endl;
}

void SctpNotification::PrintAuthenticationIndication(union sctp_notification *notification)
{
	sctp_authkey_event  *sctpAuthEvent;
	sctpAuthEvent = &notification->sn_authkey_event;
	
	std::cout << "[Notification]: SCTP_AUTHENTICATION_INDICATION: assoc id = " << sctpAuthEvent->auth_assoc_id << std::endl;
}

void SctpNotification::PrintSenderDryEvent(union sctp_notification *notification)
{
	sctp_sender_dry_event   *sctpSenderDryEvent;
	sctpSenderDryEvent = &notification->sn_sender_dry_event;
	
	std::cout << "[Notification]: SCTP_SENDER_DRY_EVENT: assoc id = " << sctpSenderDryEvent->sender_dry_assoc_id << std::endl;
}

void SctpNotification::Print(char* notify_buf)
{
	sctp_notification *notification;
	
	notification = (union sctp_notification *) notify_buf;
	
	switch(notification->sn_header.sn_type) {
		case SCTP_ASSOC_CHANGE: 
			PrintAssocChange(notification);
			break;
		case SCTP_PEER_ADDR_CHANGE:
			PrintPeerAddrChange(notification);
			break;
		case SCTP_REMOTE_ERROR:
			PrintRemoteError(notification);
			break;
		case SCTP_SHUTDOWN_EVENT:
			PrintShutDownEvent(notification);
			break;		
		case SCTP_SEND_FAILED:
			PrintSendFailed(notification);
			break;
		case SCTP_ADAPTATION_INDICATION:
			PrintAdaptionIndication(notification);
			break;
		case SCTP_PARTIAL_DELIVERY_EVENT:
			PrintPartialDeliveryEvent(notification);
			break;
		case SCTP_AUTHENTICATION_INDICATION:
			PrintAuthenticationIndication(notification);
			break;
		case SCTP_SENDER_DRY_EVENT:
			PrintSenderDryEvent(notification);
			break;			       			
		default:
			std::cout << "[Notification Err]: Unkown event type = " << notification->sn_header.sn_type << std::endl;
	}
}