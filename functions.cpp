// Header File for declaring fucntions

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "sha1.h"
#include "functions.h"

using namespace std;
namespace fs = std::filesystem;



string read_file_content(fs::path filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
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

string get_time_string() {
    time_t get_time = time(0);
    char buf[80];  
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&get_time));
    return string(buf);
}

string use_config() {
    ifstream configFile(".mygit/config");
    stringstream buffer;
    buffer << configFile.rdbuf();
    return buffer.str();
}

//  +++++++++++++++++++++++++  INIT  ++++++++++++++++++++++++++++

void init_command() {
    
    fs::path gitRepo = fs::current_path() / ".mygit";

    if (fs::exists(gitRepo) && fs::exists(gitRepo / "objects") && fs::exists(gitRepo / "refs" / "heads") && fs::exists(".mygit/HEAD") && fs::exists(".mygit/index") && fs::exists(".mygit/history")) {
        cout << "The repository already exists in " << gitRepo.string() << " ...";
        return;
    }

    try {

        fs::create_directory(gitRepo);
        fs::create_directories(gitRepo / "objects");
        fs::create_directories(gitRepo / "refs" / "heads");
        std::ofstream (".mygit/history");

        cout << "Creating an empty repository in " << gitRepo.string() << " ...";

        std::ofstream headFile(".mygit/HEAD");

        if (headFile.is_open()) {

            headFile << "ref: refs/heads/main";
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

//  +++++++++++++++++++++++++  LOG  +++++++++++++++++++++++++++++++

void update_history(string folderName, string fileName) {

    fs::path objectsPath =  ".mygit/objects";
    fs::path folderPath = objectsPath / folderName;
    fs::path filePath = folderPath / fileName;


    string content = read_file_content(filePath);

    ofstream file(".mygit/history" , ios::app);

    file << "commit:  " << folderName + fileName << endl << content << endl << endl;
    
}

void display_history() {

    fs::path historyPath = ".mygit/history";

    string content = read_file_content(historyPath);

    cout << content;
    

}

//  +++++++++++++++++++++++++  COMMIT  +++++++++++++++++++++++++++++++

string read_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return "";          // Return empty if file not found
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

string save_content(const string& content) {

    string hash = get_hash(content); // hash of index 

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
        printerHead << content; // tree object is completed 
    }

    return hash;

}

void commit_command() {

    fs:: path gitRepo = fs::current_path() / ".mygit"; // checks for the ./git folder
    fs:: path indexFile = ".mygit/index"; // stores address of index file 
    if ((!fs::exists(gitRepo) || fs::is_empty(indexFile))) // if git folder or indexfile does not exist
    {
        cout << "No change added to the file." << endl;
        return;
    }

    string content = read_file(indexFile.string());       // returns the content of the index file
    string tree_hash = save_content(content); // returns the hash of the index file 
    std::string main_file = ".mygit/refs/heads/main";   // returns the address of the main file that stores the hash of the latest commit
    fs::path main_file_path = ".mygit/refs/heads/main";

    if (!fs::exists(main_file_path))    // checks if main file is not empty
    {
        fs::create_directories(main_file_path.parent_path()); // creates .mygit/refs/heads if missing
        std::ofstream main_file_stream(main_file_path);
        if (!main_file_stream.is_open()) {
            std::cout << "Error: Could not create file at " << main_file_path << std::endl;
        } 
        else {
            main_file_stream << "";
            main_file_stream.close();
        }

    }
        string parent_commit = "";
        if (!fs::is_empty(main_file_path)) { // if main file is not empty, read the hash of latest commit
            parent_commit = read_file(main_file);
        }
        string committer;
        committer = use_config();
        if (committer.empty()) {
            committer = "   -   ";
        }

        string message;
        do {
            cout << "Enter the commit message: ";
            getline(cin, message);
            if (message.empty()) {
                cout << "Commit message cannot be empty. Please try again.\n";
            }
        } 
        while (message.empty());

        string commit_time = get_time_string(); // returns Timestamp
        string commit_content = "Tree: " + tree_hash + "\n";
        if (!parent_commit.empty()) { // if any previous commit exists
            commit_content += "Parent: " + parent_commit + "\n";
        }
        commit_content += "Author Name: " + committer + "\n" + "Date: " + commit_time + "\n\n" + message;            
        string commit_hash = save_content(commit_content); 
        ofstream printerHead(main_file);   // overwrite the parent commit
        if (!printerHead.is_open()) {
            cout << "Error: Could not open main file!" << endl;
            return;
        }
        printerHead << commit_hash;        // write the latest commit hash
        printerHead.close(); 
    ofstream ofs(".mygit/index", ios::trunc); // Clear the index file 
    ofs.close(); 

    string folderName = commit_hash.substr(0,2); 
    string fileName = commit_hash.substr(2);
    update_history(folderName, fileName); 
}
