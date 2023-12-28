#include <iostream>
#include <memory>
#include <EngineCore/Application.hpp>


class MyApp : public EngineCore::Application {

    int frame = 0;

    virtual void on_update() override {
        // std::cout << "Frame:" << frame++ << std::endl;
    }

};

int main() {

    MyApp App;
    auto exitCode = App.start(100, 100, "my first app");
    return exitCode;
}
