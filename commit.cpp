#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "sha1.h"

#include <filesystem>
using namespace std;
namespace fs  = std::filesystem;


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

// --- READ FILE CONTENT ---
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return "";          // Return empty if file not found
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// --- COMPUTING HASH AND CREATNG OBJECT ---
string saveContent(const string& content) {

    string hash = getHash(content); // hash of index 

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

// --- COMPUTING TIMESTAMP ---
string getTimeString() {
    time_t get_time = time(0);
    char buf[80];  
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&get_time));
    return string(buf);
}


void commit()
{

    fs:: path gitRepo = fs::current_path() / ".mygit"; // checks for the ./git folder
    fs:: path indexFile = ".mygit/index"; // stores address of index file 
    if ((!fs::exists(gitRepo) || fs::is_empty(indexFile))) // if git folder or indexfile does not exist
    {
        cout << "No change added to the file." << endl;
        return;
    }

    string content = readFile(indexFile.string());       // returns the content of the index file
    string tree_hash = saveContent(content); // returns the hash of the index file 
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
            parent_commit = readFile(main_file);
        }
        string committer;
        do {
            cout << "Enter your name: ";
            getline(cin, committer);
            if (committer.empty()) {
                cout << "Committer name cannot be empty. Please try again.\n";
            }
        } 
        while (committer.empty());

        string message;
        do {
            cout << "Enter the commit message: ";
            getline(cin, message);
            if (message.empty()) {
                cout << "Commit message cannot be empty. Please try again.\n";
            }
        } 
        while (message.empty());

        string commit_time = getTimeString(); // returns Timestamp
        string commit_content = "tree " + tree_hash + "\n";
        if (!parent_commit.empty()) { // if any previous commit exists
            commit_content += "parent " + parent_commit + "\n";
        }
        commit_content += "committer " + committer + "\n" + "date " + commit_time + "\n\n" + message;            
        string commit_hash = saveContent(commit_content); 
        ofstream printerHead(main_file);   // overwrite the parent commit
        if (!printerHead.is_open()) {
            cout << "Error: Could not main file" << endl;
            return;
        }
        printerHead << commit_hash;        // write the latest commit hash
        printerHead.close(); 
    ofstream ofs(".mygit/index", ios::trunc); // Clear the index file 
    ofs.close();          
}

int main()
{
    commit();
}

