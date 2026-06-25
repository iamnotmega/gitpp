#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "repository.hpp"

namespace gitpp {
    /* Create a new empty repository in a specified directory */
    void init(const std::string& repo) {
        /* Create a directory for the repository */
        std::filesystem::create_directory(repo);

        /* Create subdirectories */
        std::filesystem::create_directories(repo + "/.gitpp/objects");
        std::filesystem::create_directories(repo + "/.gitpp/refs/heads");

        /* Create HEAD file */
        std::ofstream(repo + "/.gitpp/HEAD") << "ref: refs/heads/master";

        std::cout << "initialized empty repository: " << repo << '\n';
    }
}