#include <filesystem>
#include <iostream>
#include "repository.hpp"

/* Print text block with helpful usage information */
void print_usage() {
    std::cout << "Usage: gitpp [-h | --help] <command> [<args>]\n\n"
              << "Available commands:\n"
              << "  init <directory>           Initialize a new, empty repository\n"
              << "Options:\n"
              << "-h, --help      Show this help message\n";

}

int main(const int argc, char* argv[]) {
    /* Ensure a subcommand has been passed, otherwise print help message */
    if (argc < 2) {
        std::cout << "Usage: gitpp <command> [<args>]\n";
        std::cout << "Try 'gitpp --help' for more options.\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "--help" || command == "-h") {
        print_usage();
        return 0;
    }
    if (command == "init") {
        if (argc < 3) {
            std::cerr << "Error: 'init' requires a target directory name.\n";
            std::cout << "Try 'gitpp --help' for more options.\n";
            return 1;
        }
        std::string repo_name = argv[2];
        gitpp::init(repo_name); /* Create empty repository with passed name */
    } else { /* Unknown command handling */
        std::cerr << "Unknown command: " << command << '\n';
        std::cout << "Try 'gitpp --help' for more options.\n";
        return 1;
    }
    return 0;
}