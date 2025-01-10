#include <bits/stdc++.h>
#include <memory>

#include "src/Interface.h"
#include "src/Backend.h"
// using namespace std;

int main(){
    std::promise<bool> frontEnd, backEnd;
    std::future<bool> frontEndFuture = frontEnd.get_future(), backEndFuture = backEnd.get_future();

    bool frontFinish, backFinish;

    GosFrontline::Backend &backend = GosFrontline::Backend::getBackend();
    GosFrontline::InterfaceCLI interface;
    auto frontendThread = std::thread([&interface](std::promise<bool> end){interface.run(); end.set_value(true);}, std::ref(frontEnd));
    auto backendThread = std::thread([&backend](std::promise<bool> end){backend.run(); end.set_value(true);}, std::ref(backEnd));
    frontendThread.join();
    backendThread.join();
    return 0;
}