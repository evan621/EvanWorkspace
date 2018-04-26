class IPort
{
public:
    virtual ~IPort() {}
    virtual void receiveMsg() = 0;
    virtual void handleFd() = 0 ;
};

class SctpPort: public IPort
{
public:
    SctpPort()
    {
    }
    ~SctpPort()
    {
    }

    virtual void receiveMsg()
    {
    }

    virtual void handleFd()
    {
    }
private:
    
};



