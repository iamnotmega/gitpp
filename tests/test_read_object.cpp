#include <iostream>

#include "object.hpp"
#include "repository.hpp"

/* Test reading objects using a prefix */
int main() {
    std::cout << "Testing object reading...\n";

    /* Create temporary repository in working directory */
    gitpp::init(".");

    std::string test_data = "git plus plus"; /* Test data for creating an object */

    /* Hash the object and write it to the disk */
    std::string full_hash = hash_object(test_data, "blob", true);

    /* Extract first 2 characters as prefix */
    std::string prefix = full_hash.substr(0, 2);

    /* Attempt to read the test object */
    try {
        auto result = read_object(prefix); /* Variable for storing the result */
        std::string data(result.second.begin(), result.second.end()); /* Object data */

        if (result.first != "blob" || data != "git plus plus") { /* If data and/or type do not match the expected values, fail the test */
            std::cerr << "test_read_object: FAIL\n";
            std::filesystem::remove_all(".gitpp");  /* Cleanup */
            return 1;
        }
        std::cout << "\033[32mtest_read_object: PASS\n";
        std::filesystem::remove_all(".gitpp");  /* Cleanup */
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "test_read_object: FAIL (" << e.what() << ")\n";
        std::filesystem::remove_all(".gitpp"); /* Cleanup */
        return 1;
    }
}