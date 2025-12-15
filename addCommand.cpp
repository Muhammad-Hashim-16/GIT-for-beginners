#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "sha1.h" // Ensure this file is next to your main.cpp

using namespace std;
namespace fs = std::filesystem;

// --- SHA1 ADAPTER ---
string getHash(const string& input) {

    sha1::SHA1 s;
    s.processBytes(input.c_str(), input.size());

    uint32_t digest[5];
    s.getDigest(digest);

    stringstream ss;
    for(int i = 0; i < 5; ++i) {
        ss << hex << setfill('0') << setw(8) << digest[i];
    }
    return ss.str();

}


// --- FILE READER ---
string readFile(const string& filename) {

    ifstream file(filename);
    if (!file.is_open()) {
        return "";          // Return empty if file not found
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();

}

// --- SAVING THE BLOB ---
string saveBlob(const string& content) {

    string hash = getHash(content);

    string folderName = hash.substr(0,2);
    string fileName = hash.substr(2);

    fs::path objectDirectory = ".mygit/objects";
    fs::path targetDirectory = objectDirectory / folderName;
    fs::path targetFile = targetDirectory / fileName;

    if(!fs::exists(targetDirectory)) {
        fs::create_directories(targetDirectory);
    }

    if(!fs::exists(targetFile)) {
        ofstream printerHead (targetFile);
        printerHead << content;
    }

    return hash;

}

// --- UPDATE INDEX FUNCTION ---
void updateIndex(const string& filename, const string& hash) {

    fs::path indexPath = ".mygit/index";
    ofstream indexFile (indexPath, ios::app);

    if(!indexFile.is_open()) {
        cout << "Error: Could not open .mygit/index" << endl;
        return;
    }

    indexFile << filename << " " << hash << endl;

}

// --- THE INIT COMMAND FUNCTON --- 
void createRepo() {

    // 1. Check if " .mygit " it already exists
    if (fs::exists(".mygit")) {
        cout << "Error: Repository already exists." << endl; 
        return;
    }

    // 2. If it does not exist create it
    fs::create_directory(".mygit");
    fs::create_directories(".mygit/objects");
    fs::create_directories(".mygit/refs/heads");

    // 
    ofstream headFile(".mygit/HEAD");
    if (headFile.is_open()) {
        headFile << "ref: refs/heads/master";
    } else {
        cout << "Error: Could not create HEAD file." << endl;
        return;
    }

    cout << "Initialized empty git repository in .mygit/" << endl;

}

// --- THE ADD COMMAND FUNCTION ---
void add(int argc, char* argv[]) {

    // Safety Check: Did they provide a filename?
    if (argc < 3) {
        cout << "Error: Please specify a file to add." << endl;
         return;
    }

    // Looping to add multiple files
    for(int i=2; i<argc; i++) {

        string filename = argv[i];
    
        // 1. Check if file exists
        if (!fs::exists(filename)) {
            cout << endl << "Error: File '" << filename << " does not exist." << endl;
            continue;
        }

        // 2. Read the content
        string content = readFile(filename);

        // 3. Hash and Save (Blob)
        string hash = saveBlob(content);

        // 4. Update the Index list
        updateIndex(filename, hash);

        cout << endl << "Added " << filename << " (Hash: " << hash << ")" << endl;

    }
}

// Our MAIN 
int main(int argc, char* argv[]) {
 
    // Safety Check: Did user type a command?
    if (argc < 2) {

        cout << "Error: No command provided." << endl;
        return 1;

    }

    string command = argv[1];

    // --- THE INIT COMMAND ---
    if (command == "init") {

        createRepo();

    }

    // --- THE ADD COMMAND ---
    else if (command == "add") {
        
        add(argc, argv);

    }

    return 0;
}