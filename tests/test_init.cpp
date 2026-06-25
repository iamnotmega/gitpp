#include <iostream>
#include <filesystem>
#include <cassert>

#include "repository.hpp"

/* Test repository initialization */
int main() {
    std::cout << "Testing repository initialization...\n";

    /* Create repository in working directory */
    gitpp::init(".");

    /* Check if directories exist */
    assert(std::filesystem::exists(".gitpp") && "FAIL: .gitpp directory missing");
    assert(std::filesystem::exists(".gitpp/objects") && "FAIL: .gitpp/objects directory missing");
    assert(std::filesystem::exists(".gitpp/refs") && "FAIL: .gitpp/refs directory missing");
    assert(std::filesystem::exists(".gitpp/HEAD") && "FAIL: .gitpp/HEAD file missing");

    /* Clean up temporary repository */
    std::filesystem::remove_all(".gitpp");

    /* Report success */
    std::cout << "\033[32mtest_init: PASS\n";
    return 0;
}