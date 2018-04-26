


void main()
{
    MsgPollAndDispather msgPollAndDispatcher;

    SctpProxyApp sctpProxyApp;

    sctpProxyApp.start(msgPollAndDispatcher)

    while(1)
    {
        msgPollAndDispatcher.run();
    }
}
