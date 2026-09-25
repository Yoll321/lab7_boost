#include "view.hpp"
#include <iostream>

void View::display_duplicates(const std::vector<std::vector<fs::path>>& duplicates)
{
    if (duplicates.empty()) {
        std::cout << "No duplicates found\n";
        return;
    }

    std::cout << "Found duplicates:\n";
    for (const auto& it : duplicates) {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        for (const auto& path: it) {
            std::cout << path << '\n';
        }
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    return;
}