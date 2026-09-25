#pragma once

#include <memory>

#include "model.hpp"
#include "view.hpp"
#include "boost_includes.hpp"

class Controller {
private:
    std::shared_ptr<IModel> model_;
    std::shared_ptr<IView>  view_;

public:
    explicit Controller(std::shared_ptr<Model>& model, std::shared_ptr<View>& view)
        : model_(model), view_(view) {}
    void saveArgs(int argc, char* argv[]);
    void getFilenames(void);
    void findDuplicates(void);
};