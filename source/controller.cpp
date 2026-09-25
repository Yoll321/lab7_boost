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

void Controller::findDuplicates(void)
{
    model_->findDuplicates();
    view_->display_duplicates(model_->getDuplicatesTable());
    return;
}
