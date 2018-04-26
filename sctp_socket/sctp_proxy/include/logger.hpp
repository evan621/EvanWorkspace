#pragma once

#include <spdlog/spdlog.h>
#include <memory>
//if(::logger::internals::globalLogEnabled()) \

#define LOG_MSG_IMPL(loggerInstance, operate, ...)          \
        loggerInstance.operate(__VA_ARGS__)

#define LOG_INFO_MSG(...) LOG_MSG_IMPL(logger::wrapper::Instance(), info, __VA_ARGS__)
#define LOG_DEBUG_MSG(...) LOG_MSG_IMPL(logger::wrapper::Instance(), debug, __VA_ARGS__)
#define LOG_ERROR_MSG(...) LOG_MSG_IMPL(logger::wrapper::Instance(), error, __VA_ARGS__)


#define LOG_ENABLE_DBG() LOG_MSG_IMPL(logger::wrapper::Instance(), set_level, spdlog::level::debug)


namespace logger
{

class configuration{
public:
    static configuration& Instance()
    {
        static configuration config;

        return config;
    }

    spdlog::sink_ptr getSink()
    {       
        return sink;
    }

private:
    configuration()
    {
        sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    }

    ~configuration()
    {}

private:
    spdlog::sink_ptr sink;
    
};

class wrapper{
public:
    static spdlog::logger& Instance() {
        static wrapper log_wrapper;

        return *(log_wrapper.log);
    }

private:
    std::unique_ptr<spdlog::logger> log;
    
private:
    wrapper()
    {
        auto sink = configuration::Instance().getSink();
    
        log = std::make_unique<spdlog::logger>("logger", sink);
        
        printf("Wrapper constructor, %p, %p\n", sink.get(), log.get());
    
        log->set_level(spdlog::level::info);
        //logger->set_pattern();
        log->flush_on(spdlog::level::err);
    }
    ~wrapper()
    {
    }
    
};
}
