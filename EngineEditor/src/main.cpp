#include <iostream>
#include <EngineCore/Application.hpp>
#include <EngineCore/Logs.hpp>


class MyApp : public EngineCore::Application {

    int frame = 0;

    virtual void on_update() override {
        // std::cout << "Frame:" << frame++ << std::endl;
    }

};

int main() {
    MyApp App;
    auto exitCode = App.start(1024, 768, "my first app");
    return exitCode;
}
