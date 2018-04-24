#include "sctpnotification.h"

#define INET6_ADDRSTRLEN 46

void PrintAssocChange(union sctp_notification *notification)
{
	struct sctp_assoc_change *sctpAssociationChange;
	char* str;
	
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
	
	printf("[Notification]: SCTP_ASSOC_CHANGE: '%s' received with assoc id = 0x%x\n", str, sctpAssociationChange->sac_assoc_id);
}

void PrintPeerAddrChange(union sctp_notification *notification)
{
	char* 	str;
	struct 	sctp_paddr_change *sctpPeerAddChange;
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
			printf("[Notification]: UNKNOWN state = 0x%x\n", sctpPeerAddChange->spc_state);
			break;
	}

	// try to get the IP address 	
	if (sctpPeerAddChange->spc_aaddr.ss_family == AF_INET) {
		struct sockaddr_in* sin = (struct sockaddr_in*) &sctpPeerAddChange->spc_aaddr;
		ap = inet_ntop(AF_INET, &sin->sin_addr, addrbuf, INET6_ADDRSTRLEN);
	} else {
		struct sockaddr_in6* sin6 = (struct sockaddr_in6 *) &sctpPeerAddChange->spc_aaddr;
		ap = inet_ntop(AF_INET6, &sin6->sin6_addr, addrbuf, INET6_ADDRSTRLEN);
	}
	printf("[Notification]: ip = %s\n", addrbuf);
	printf("[Notification]: SCTP_PEER_ADDR_CHANGE: '%s' received with assoc id = 0x%x\n", str, sctpPeerAddChange->spc_assoc_id);
}

void PrintRemoteError(union sctp_notification *notification)
{
	struct sctp_remote_error *sctpRemoteErr;
	sctpRemoteErr = &notification->sn_remote_error;
	printf("[Notification]: SCTP_REMOTE_ERROR received with assoc id = 0x%x error= %d!\n",
			sctpRemoteErr->sre_assoc_id, sctpRemoteErr->sre_error);
}

void PrintSendFailed(union sctp_notification *notification)
{
	struct sctp_send_failed *sctpSendFailed;
	sctpSendFailed = &notification->sn_send_failed;
	printf("[Notification]: SCTP_SEND_FAILED received with assoc id = 0x%x error = %d !\n",
			sctpSendFailed->ssf_assoc_id, sctpSendFailed->ssf_error);
}

void PrintAdaptionIndication(union sctp_notification *notification)
{
	struct sctp_adaptation_event *sctpAdaptEvent;
	sctpAdaptEvent = &notification->sn_adaptation_event;
	printf("[Notification]: SCTP_ADAPTION_INDICATION: 0x%x received!\n",
			sctpAdaptEvent->sai_adaptation_ind);
}

void PrintPartialDeliveryEvent(union sctp_notification *notification)
{
	struct sctp_pdapi_event *sctpPdapiEvent;
	sctpPdapiEvent = &notification->sn_pdapi_event;
	
	if(sctpPdapiEvent->pdapi_indication == SCTP_PARTIAL_DELIVERY_ABORTED)
		printf("[Notification]: SCPT_PARTIAL_DELIVERY_ABORTED!");
	else
		printf("[Notification]: Unkown SCPT_PARTIAL_DELIVERY_EVENT!");

}

void PrintShutDownEvent(union sctp_notification *notification)
{
	struct sctp_shutdown_event *sctpShutdownEvent;
	sctpShutdownEvent = &notification->sn_shutdown_event;
	
	printf("[Notification]: SCTP_SHUTDOWN_EVENT: assoc id = 0x%x\n", sctpShutdownEvent->sse_assoc_id);
}

void PrintAuthenticationIndication(union sctp_notification *notification)
{
	struct sctp_authkey_event  *sctpAuthEvent;
	sctpAuthEvent = &notification->sn_authkey_event;
	
	printf("[Notification]: SCTP_AUTHENTICATION_INDICATION: assoc id = 0x%x\n", sctpAuthEvent->auth_assoc_id);
}

void PrintSenderDryEvent(union sctp_notification *notification)
{
	struct sctp_sender_dry_event   *sctpSenderDryEvent;
	sctpSenderDryEvent = &notification->sn_sender_dry_event;
	
	printf("[Notification]: SCTP_SENDER_DRY_EVENT: assoc id = 0x%x\n", sctpSenderDryEvent->sender_dry_assoc_id);
}

void PrintSctpNotification(char* notify_buf)
{
	union sctp_notification *notification;
	
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
			printf("[Notification Err]: Unkown event type = 0x%x\n", notification->sn_header.sn_type);
	}
	
}