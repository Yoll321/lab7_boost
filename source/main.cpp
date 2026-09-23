#include <iostream>
#include <vector>
#include <string>
#include <memory>


#include "controller.hpp"
#include "model.hpp"
#include "view.hpp"


int main(int argc, char* argv[]) {
    std::shared_ptr<Model> model = std::make_shared<Model>();
    std::shared_ptr<View> view = std::make_shared<View>();
    Controller controller(model, view);

    controller.saveArgs(argc, argv);
    controller.getFilenames();

    return 0;
}