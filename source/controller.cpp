#include "controller.hpp"

void Controller::saveArgs(int argc, char *argv[])
{
    model_->getVM(argc, argv);
    return;
}

void Controller::getFilenames(void)
{
    model_->getFilenames();
    return;
}
