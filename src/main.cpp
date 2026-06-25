#include <filesystem>
#include <iostream>
#include "repository.hpp"
#include "object.hpp"

// TODO: Implement argument handling for commands
int main() {
    std::string name = "gitppTest";
    gitpp::init(name);

    std::filesystem::current_path(name);

    std::string test_data = "hello world";
    std::string generated_hash = hash_object(test_data, "blob", true);

    std::cout << "Generated Hash: " << generated_hash << std::endl;
    return 0;
}