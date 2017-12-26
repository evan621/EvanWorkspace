
class SctpEndpoint
{
public:
	virtual ~SctpEndpoint() = default;
	virtual void SendMsg() = 0;
	virtual void RegisterMsgHandler() = 0;
	virtual int StartPoolForMsg(int timeout) = 0;
};