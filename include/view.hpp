#pragma once
#include <vector>

#include "boost_includes.hpp"

class IView {
public:
    virtual ~IView() = default;
    virtual void display_duplicates(const std::vector<std::vector<fs::path>> &duplicates) = 0;
}; 

class View : public IView {
public:
    void display_duplicates(const std::vector<std::vector<fs::path>> &duplicates) override;
};