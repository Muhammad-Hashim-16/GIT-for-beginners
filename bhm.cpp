#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

void init_command() {
    
    fs::path gitRepo = fs::current_path() / ".mygit";

    if (fs::exists(gitRepo) && fs::exists(gitRepo / "objects") && fs::exists(gitRepo / "refs" / "heads") && fs::exists(".mygit/HEAD") && fs::exists(".mygit/index")) {
        cout << "The repository already exists in " << gitRepo.string() << " ...";
        return;
    }

    try {

        fs::create_directory(gitRepo);
        fs::create_directories(gitRepo / "objects");
        fs::create_directories(gitRepo / "refs" / "heads");
        cout << "Creating an empty repository in " << gitRepo.string() << " ...";

        std::ofstream headFile(".mygit/HEAD");

        if (headFile.is_open()) {

            headFile << "ref: refs/heads/master";
            headFile.close();

        } else {

            cerr << "ERROR: Could not create HEAD File." << endl;
            return;

        }

        std::ofstream indexFile(".mygit/index");
        indexFile.close();

    } catch (...) {

        cerr << "Initialization failed: Something went wrong(Maybe permission denied!)";

    }

}

void add_command(fs::path filename) {

// Generate the string of blob file


    
// Generate the name (hash) of the blob



// 



}

int main (int argc, char* argv[]) {

    vector<string> args;
    for (int i=0; i<argc; i++) {
        args.push_back(argv[i]);
    }

    if (argc<2) {
        cout << "Enter a valid command!";
        return 1;
    }

    if (args[1] == "create") {
        
        init_command();

    } else if (args[1] == "add") {

        fs::path filename = args[2];

        if (fs::exists(filename)) {
            add_command(filename);
        } else {
            cout << filename << " doesn't exist.";
        }

        // add_command();

    } else if (args[1] == "commit") {
        cout << "Commiting the files...";
    } else if (args[1] == "log") {
        cout << "Showing the history of files...";
    } else if (args[1] == "status") {
        cout << "Showing the status...";
    } else {
        cout << "Invalid command!";
    }
    return 0;

}