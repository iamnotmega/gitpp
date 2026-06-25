#include <filesystem>
#include <iostream>
#include "repository.hpp"

void print_usage() {
    std::cout << "Usage: gitpp <command> [<args>]\n\n"
              << "Available commands:\n"
              << "  init <directory>           Initialize a new, empty repository\n";
}

int main(int argc, char* argv[]) {
    /* Ensure a subcommand has been passed, otherwise print the usage */
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "init") {
        if (argc < 3) {
            std::cerr << "Error: 'init' requires a target directory name.\n";
            print_usage();
            return 1;
        }
        std::string repo_name = argv[2];
        gitpp::init(repo_name); /* Create empty repository with passed name */
    } else { /* Unknown command handling */
        std::cerr << "Unknown command: " << command << '\n';
        print_usage();
        return 1;
    }
    return 0;
}