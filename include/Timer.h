#pragma once

#include <boost/log/trivial.hpp>
#include <thread>

#include "Parser.h"
#include "Globals.h"

namespace Timer
{
    void create(std::unique_ptr<ThreadData> &data);
}


void Timer::create(std::unique_ptr<ThreadData> &data)
{
    using namespace std::chrono;

    if (Globals::timeout <= 0) {
        return;
    }

    while (Globals::timeout > 0 && (!data || !data->isTerminated)) {
        static const int DATA_TIMEOUT = 50;

        std::this_thread::sleep_for(milliseconds{DATA_TIMEOUT});
        Globals::timeout -= DATA_TIMEOUT;
    }

    data->isTerminated = true;

    try {
        data->currentProcess.terminate();
    } catch (std::exception &exception) {
        BOOST_LOG_TRIVIAL(error) << exception.what();
    }

    BOOST_LOG_TRIVIAL(info) << "Timeout";
}