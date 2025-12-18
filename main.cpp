#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "sha1.h"

using namespace std;
namespace fs = std::filesystem;

//  +++++++++++++++++++++++++  INIT  ++++++++++++++++++++++++++++

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
        std::ofstream configFile(".mygit/config");
        configFile.close();

    } catch (...) {

        cerr << "Initialization failed: Something went wrong (Maybe permission denied!)";

    }

}

//  +++++++++++++++++++++++++  ADD  +++++++++++++++++++++++++++++

string get_hash(string content) {
    sha1::SHA1 s;  // Turn the machine on
    
    // Feed the string to SHA-1
    s.processBytes(content.c_str(), content.size()); 

    // Get an array of decimal numbers
    uint32_t digest[5]; 
    s.getDigest(digest); 

    // Convert the decimal number into hexadecimal number-40 characters hash code
    stringstream ss;
    for(int i = 0; i < 5; i++) {
        ss << hex << setfill('0') << setw(8) << digest[i];
    }
    
    return ss.str(); // Return the "filename"
}

string read_file_content(fs::path filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void create_blob_file(string stringHash, string content) {

    string folderName = stringHash.substr(0,2);
    string blobFilename = stringHash.substr(2);

    fs::path objectDirectory = ".mygit/objects";
    fs::path targetDirectory = objectDirectory / folderName;
    fs::path targetFile = targetDirectory / blobFilename;

    if (!fs::exists(targetDirectory)) {
        fs::create_directories(targetDirectory);
    }

    std::ofstream blobFile(targetFile);
    if (!blobFile.is_open()) {
        cout << "Error! Couldn't create the file" << blobFilename << endl;
        return ;
    }
    blobFile << content;
    blobFile.close();
}

vector<vector<string>> index_to_vector() {
    vector<vector<string>> indexTableVector;
    ifstream indexFile (".mygit/index");
    string line;

    while (std::getline(indexFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        vector<string> row;
        string rowSegment;
        while (ss >> rowSegment) {
            row.push_back(rowSegment);
        }
        indexTableVector.push_back(row);
    }
    return indexTableVector;
}

void vector_to_index(vector<vector<string>> indexTableVector) {
    fs:: path indexPath = ".mygit/index";
    ofstream indexFile(indexPath, ios::trunc);
    if (!indexFile.is_open()) {
        cerr << "ERROR! Couldn't open index file";
        return;
    }
    for (int i=0; i<indexTableVector.size(); i++) {
        indexFile << indexTableVector[i][0] << " " << indexTableVector[i][1] << endl;
    }
}

bool check_for_repo() {
    return fs::exists(".mygit") && fs::exists(".mygit/HEAD");
}

void add_command(int argc, char* argv[]) {

// Check if the repo is already initialized or not?

    if (!check_for_repo()) {
        cout << "Error: Not a git repository!" << endl;
        cout << "Try running './main' first!" << endl;
        return;
    }

// Convert char array into string vector

    vector<string> args;
    for (int i=0; i<argc; i++) {
        args.push_back(argv[i]);
    }

// If no file given

    if (argc<3) {
        cout << "Please give a filename!";
        return;
    }

// DONE 1- If the file is already added and is changed. ---> Hash is changed so: Two available options: 1- Make a new blob of it. 2- Overwrite its hash in index file.
// DONE 2- If the file is already added and not changed. ---> Simply ignore it by comparing hash.
// DONE 3- If the file doesn't exist there. ---> Show error on just that file ke the file doesn't exist and ADD the rest of the files.
// DONE 4- Gave no file at all. ---> Show error ke you gave no file at all.

    vector<vector<string>> indexTableVector = index_to_vector();

// -------------- Running loop for all the files --------------

    for (int i=2; i<argc; i++) {

    // Find the path of files

        fs::path filename = args[i];
        string filenameString = args[i];

    // Check if that file exists or not

        if (!fs::exists(filename)) {
            cout << filename << " doesn't exists! Adding other files..." << endl;
            continue;
        }

    // Generate the string of blob file

        string stringContent = read_file_content(filename);

    // Generate the name (hash) of the blob

        string stringHash = get_hash(stringContent);

    // Save the blob file in desired path

        create_blob_file(stringHash, stringContent);

    // Update the hash of that file whose content is changed        

       bool found = false;

        for (int row=0; row<indexTableVector.size(); row++) {
            if (filenameString == indexTableVector[row][0]) {
                found = true; 

                string existingHash = indexTableVector[row][1];
                
                if (existingHash == stringHash) {
                    cout << filenameString << " : No changes detected. Skipping..." << endl;
                } 
                else {
                    cout << filenameString << " : File changed. Updating index." << endl;
                    indexTableVector[row][1] = stringHash;
                }
                
                break;
            }
        }

        if (!found) {
            cout << "Adding new file: " << filenameString << endl;
            vector<string> newRow;
            newRow.push_back(filenameString);
            newRow.push_back(stringHash);
            indexTableVector.push_back(newRow);
        }

    }

    vector_to_index(indexTableVector);

    cout << "All files added successfully!";

}

//  +++++++++++++++++++++++++  CONFIG  ++++++++++++++++++++++++++++

void config_command(int argc, vector<string> args) {
    ofstream configFile(".mygit/config");
    if (!configFile.is_open()) {
        cout << "Error!" << endl;
        return;
    }
    string authorName;
    for (int i=3; i<args.size(); i++) {
        if (i>3) {
            authorName = authorName + " " + args[i];
        } else {
            authorName = args[i];
        }
    }
    configFile << authorName;
    configFile.close();
}

string use_config() {
    ifstream configFile(".mygit/config");
    stringstream buffer;
    buffer << configFile.rdbuf();
    return buffer.str();
}



int main (int argc, char* argv[]) {

    vector<string> args;
    for (int i=0; i<argc; i++) {
        args.push_back(argv[i]);
    }

    if (argc<2) {
        cout << "Compiled successfully!";
        return 1;
    }

    if (args[1] == "create") {
        
        init_command();

    } else if (args[1] == "add") {

        add_command(argc, argv);

    } else if (args[1] == "commit") {
        cout << "Commiting the files..." << endl << args[2] << endl << args[3];
    } else if (args[1] == "config") {

        if (argc>3 && args[2] == "user.name") {
            config_command(argc, args);
        } else {
            cout << "Error: Type 'user.name' OR type the name!";
        }

    } else if (args[1] == "log") {
        cout << "Showing the history of files...";
    } else if (args[1] == "status") {
        cout << "Showing the status...";
    } else {
        cout << "Invalid command!";
    }
    return 0;

}