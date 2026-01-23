// Header files and libraries

#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "sha1.h"
#include "functions.h"

using namespace std;
namespace fs = std::filesystem;

// Command line arguments -->

int main (int argc, char* argv[]) {

    vector<string> args;  // Making a vector string of the char array

    for (int i=0; i<argc; i++) {
        args.push_back(argv[i]);
    }

    if (argc<2) {
        cout << "Compiled successfully!";
        return 1;
    }

    // Checking for each command

    if (args[1] == "create") {
        
        init_command();

    } else if (args[1] == "add") {

        if (argc>2 && args[2] == ".") {
            add_all_command();
        } else {
            add_command(argc, args);
        }

    } else if (args[1] == "save") {

        commit_command();

    } else if (args[1] == "set") {

        if (argc>3 && args[2] == "user.name") {
            config_command(argc, args);
        } else {
            cout << "Error: Type 'user.name' OR type the name!";
        }

    } else if (args[1] == "help") {
        
        help_command();

    } else if (args[1] == "history") {

        display_history();

    }  else if (args[1] == "undo") {

       reset_command(args);

    } else {
        cout << "Invalid command!" << endl;
        cout << "Try typing 'bhm help' for Command Help Menu." << endl;
    }
    return 0;

}