#ifndef _WORKER
#define _WORKER

#include <stdio.h>
#include "IoMultiplex.hpp"
#include "spdlog/spdlog.h"
#include <memory>

class worker
{
public:
    worker();
    ~worker();
    
    void process();
private:
    std::shared_ptr<IoMultiplex> io_multi;
    std::shared_ptr<spdlog::logger> logger;
};

#endif