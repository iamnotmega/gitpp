#include <iostream>
#include <filesystem>
#include <cassert>

#include "object.hpp"
#include "repository.hpp"

/* Test finding objects using a prefix */
int main() {
    std::cout << "Testing object finding...\n";

    /* Create temporary repository in working directory */
    gitpp::init(".");

    std::string test_data = "git plus plus"; /* Test data for creating an object */

    /* Hash the object and write it to the disk */
    std::string full_hash = hash_object(test_data, "blob", true);

    /* Extract first 2 characters as prefix */
    std::string prefix = full_hash.substr(0, 2);

    /* Attempt to search for the test object */
    try {
        std::filesystem::path found_path = find_object(prefix);

        std::cout << "\033[32mtest_find_object: PASS (found using prefix " << prefix << ")\n";
        std::filesystem::remove_all(".gitpp");
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "test_find_object: FAIL (" << e.what() << ")\n";
        std::filesystem::remove_all(".gitpp");
        return 1;
    }
}