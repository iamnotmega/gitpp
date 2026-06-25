#include <iostream>
#include <filesystem>

#include "object.hpp"
#include "repository.hpp"

/* Test object hashing */
int main() {
    std::cout << "Testing object hashing...\n";

    /* Create temporary repository in working directory */
    gitpp::init(".");

    std::string test_data = "git plus plus"; /* Placeholder data for testing */
    std::string expected_hash = "fd5d89441e40ca5341c6066733ba04343b42d59b"; /* Expected hash calculated externally */

    /* Calculate hash and compare it against expected_hash */
    std::string generated_hash = hash_object(test_data, "blob", true);
    if (generated_hash == expected_hash) {
        std::cout << "\033[32mtest_hash_object: PASS (Got " << generated_hash << ", expected " << expected_hash << ")\n";
        std::filesystem::remove_all(".gitpp"); /* Clean up temporary repository */
        return 0;
    }
    std::cerr << "test_hash_object: FAIL (Got " << generated_hash << ", expected " << expected_hash << ")\n";
    std::filesystem::remove_all(".gitpp");
    return 1;
}
