#define DEBUG DEBUG

#include <bits/stdc++.h>
#include <memory>

#include "src/Interface.h"
#include "src/Backend.h"
#include "src/Logger.h"

// using namespace std;

int main()
{
    std::promise<bool> frontEnd, backEnd;
    std::future<bool> frontEndFuture = frontEnd.get_future(), backEndFuture = backEnd.get_future();

    bool frontFinish = false, backFinish = false;

    GosFrontline::Backend &backend = GosFrontline::Backend::getBackend();
    GosFrontline::InterfaceCLI interface;
    std::shared_ptr<GosFrontline::Logger> log = std::make_shared<GosFrontline::Logger>("Frontend");
    std::shared_ptr<GosFrontline::Logger> log2 = std::make_shared<GosFrontline::Logger>("Backend");

    interface.setLogger(log);
    backend.setLogger(log2);

    auto frontendThread = std::thread([&interface](std::promise<bool> end)
                                      {interface.run(); end.set_value(true); }, std::move(frontEnd));
    auto backendThread = std::thread([&backend](std::promise<bool> end)
                                     {backend.run(); end.set_value(true); }, std::move(backEnd));

    while (not(frontFinish and backFinish))
    {
        if ((not frontFinish) and frontEndFuture.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
        {
            frontFinish = true;
            log->log("Got Frontend Finish Signal.");
        }

        if ((not backFinish) and backEndFuture.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
        {
            backFinish = true;
            log2->log("Got Backend Finish Signal.");
        }
        log->run();
        log2->run();
    }

    frontendThread.join();
    backendThread.join();
    return 0;
}