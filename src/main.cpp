#include <filesystem>
#include <iostream>

#include "object.hpp"
#include "repository.hpp"

/* Print text block with helpful usage information */
void print_usage() {
    std::cout << "Usage: gitpp <command> [<args>]\n\n"
              << "Available commands:\n"
              << "  init <directory>           Initialize a new, empty repository\n"
              << "  cat-file <mode> <sha1>     Provide formatted data from a repository object hash\n"
              << "Options:\n"
              << "-h, --help      Show this help message\n"
              << "-v, --version      Print version information\n";

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
    if (command == "--version" || command == "-v") {
        std::cout << "git++ version " << GITPP_VERSION << "-unreleased" << '\n';
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
    } else if (command == "cat-file") {
        if (argc < 4) {
            std::cerr << "Error: 'cat-file' requires a file mode and a SHA-1 hash.\n";
            std::cout << "Try 'gitpp --help' for more options.\n";
        }
        /* If arguments are correct, proceed with command */
        std::string mode = argv[2];
        std::string sha1 = argv[3];
        cat_file(mode, sha1);
    } else { /* Unknown command handling */
        std::cerr << "Unknown command: " << command << '\n';
        std::cout << "Try 'gitpp --help' for more options.\n";
        return 1;
    }
    return 0;
}