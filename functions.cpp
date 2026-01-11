// Header File for declaring fucntions

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

//  +++++++++++++++++++++++++  INIT  ++++++++++++++++++++++++++++

void init_command() {
    
    fs::path gitRepo = fs::current_path() / ".mygit";

    // Checking if the repo already exists

    if (fs::exists(gitRepo) && fs::exists(gitRepo / "objects") && fs::exists(gitRepo / "refs" / "heads") && fs::exists(".mygit/HEAD") && fs::exists(".mygit/index") && fs::exists(".mygit/history")) {
        cout << "The repository already exists in " << gitRepo.string() << " ...";
        return;
    }

    // Catching if there is any error in opening the file

    try {

        // Defining the directiories

        fs::create_directory(gitRepo);
        fs::create_directories(gitRepo / "objects");
        fs::create_directories(gitRepo / "refs" / "heads");
        std::ofstream (".mygit/history");

        cout << "Creating an empty repository in " << gitRepo.string() << " ...";

        // Creating the files

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
        std::ofstream mainFile(".mygit/refs/heads/main");
        mainFile.close();

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

    // String slicing for the folder name and file name

    string folderName = stringHash.substr(0,2);
    string blobFilename = stringHash.substr(2);

    fs::path objectDirectory = ".mygit/objects";
    fs::path targetDirectory = objectDirectory / folderName;
    fs::path targetFile = targetDirectory / blobFilename;

    if (!fs::exists(targetDirectory)) {
        fs::create_directories(targetDirectory);
    }

    // Creating the blob file

    std::ofstream blobFile(targetFile);
    if (!blobFile.is_open()) {
        cout << "Error! Couldn't create the file" << blobFilename << endl;
        return ;
    }
    blobFile << content;
    blobFile.close();
}

string read_file_content(fs::path filename) {

    // Getting the string of file content

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

    // Breaking the file content into lines on the basis of ENTER and then on SPACEBARS

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
    // Return the 2d string vector
    return indexTableVector;
}

void vector_to_index(vector<vector<string>> indexTableVector) {

    // Make index file from the vector

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

    // Checking if the repo doesn't exist

    return fs::exists(".mygit") && fs::exists(".mygit/HEAD");
}

void add_command(int argc, vector<string> args) {

// Check if the repo is already initialized or not?

    if (!check_for_repo()) {
        cout << "Error: Not a git repository!" << endl;
        cout << "Try running 'bhm create' first!" << endl;
        return;
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
            cout << filename << " doesn't exist! Adding other files..." << endl;
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

void add_all_command() {

    vector<vector<string>> indexTableVector = index_to_vector();

    bool indexChanged = false;

    for (const auto& entry : fs::directory_iterator(".")) {

        if (!fs::is_regular_file(entry.path())) continue;

        fs::path filePath = entry.path();
        string filenameString = filePath.string();

        if (filenameString == "bhm" || filenameString == "bhm.exe") continue;

        string content = read_file_content(filePath);
        string hash = get_hash(content);
        create_blob_file(hash, content);

        bool found = false;

        for (int row=0; row<indexTableVector.size(); row++) {
            if (filenameString == indexTableVector[row][0]) {
                found = true; 

                string existingHash = indexTableVector[row][1];
                
                if (existingHash == hash) {
                    cout << filenameString << " : No changes detected. Skipping..." << endl;
                } 
                else {
                    cout << filenameString << " : File changed. Updating index." << endl;
                    indexTableVector[row][1] = hash;
                }
                
                break;
            }
        }

        if (!found) {
            cout << "Adding new file: " << filenameString << endl;
            vector<string> newRow;
            newRow.push_back(filenameString);
            newRow.push_back(hash);
            indexTableVector.push_back(newRow);
        }

    }

    vector_to_index(indexTableVector);

}

//  +++++++++++++++++++++++++  CONFIG  ++++++++++++++++++++++++++++

void config_command(int argc, vector<string> args) {
    ofstream configFile(".mygit/config");
    if (!configFile.is_open()) {
        cout << "Error!" << endl;
        return;
    }

    // Give the author name to the config file

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

    // Use this name in every commit

    ifstream configFile(".mygit/config");
    stringstream buffer;
    buffer << configFile.rdbuf();
    return buffer.str();
}

//  +++++++++++++++++++++++++  LOG  +++++++++++++++++++++++++++++++

void update_history(string saveHash, string saveContent) {

    string historyFile = ".mygit/history";
    ofstream file(historyFile, ios:: app);  
    file << "Commit: " << saveHash << "\n";
    file << saveContent << "\n\n";
    return;
    
}

void display_history() {

    // Display the history whenever wanted

    fs::path historyPath = ".mygit/history";

    string content = read_file_content(historyPath);

    cout << content;
    
}

//  +++++++++++++++++++++++++  COMMIT  +++++++++++++++++++++++++++++++

string get_timestamp() {
    time_t get_time = time(0);
    char buf[80];  
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&get_time));
    return string(buf);
}

void commit_command() {

    fs::path indexFile = ".mygit/index";
    fs::path mainFile = ".mygit/refs/heads/main";
    fs::path configFile = ".mygit/config";
    fs::path historyFile = ".mygit/history";
    if(!check_for_repo()) //if repo does not exist
    {
        cout << "Error: Not a git repository!" << endl;
        cout << "Try running 'bhm create' first!" << endl;
        return;
    }
    if(fs::is_empty(indexFile)) // if index file is empty
    {
        cout << "No files added to save." << endl;
        cout << "Use 'bhm add (filename)' before saving." << endl;
        return;
    }

    string treeContent = read_file_content(indexFile); // string of index file 
    string treeHash = get_hash(treeContent); // hash of index content 

    // Check if there are any changes to commit
    if(!fs::is_empty(mainFile)) 
    {
        
        string mainContent = read_file_content(mainFile); // hash of last commit 
        string indexHash;
        fs::path findCommit = ".mygit/objects/" + mainContent.substr(0,2) + "/" + mainContent.substr(2); // path of last commit object
        ifstream commitContent(findCommit); 
        if (!commitContent) {
        cout << "Error: Commit object not found!" << endl;
        return;
        }
        string line;
        while (getline(commitContent, line))  // parse commit object to extract tree hash
        { 
            if(line.find("Tree:") == 0)
            {
                indexHash = line.substr(6,45);
                break;
            }
        }
        if (indexHash == treeHash) // same tree snapshot as last commit -> nothing new to commit
        {
            cout << "No changes to commit!" << endl;
            return;
        }
    }
    
    create_blob_file(treeHash,treeContent); //store tree object
    //Parent Commit
    string parentCommit = "   -   ";
    if(!fs::is_empty(mainFile))
    {
        parentCommit = read_file_content(mainFile);
    }
    //Timestamp
    string timeStamp = get_timestamp();
    //Committer
    string committer = "   -   ";
    if(!fs::is_empty(configFile))
    {
        committer = read_file_content(configFile);
    }
    //Message 
    string message;
    do 
    {
        cout << "Enter the commit message: ";
        getline(cin, message);
        if (message.empty()) 
        {
            cout << "Commit message cannot be empty. Please try again.\n";
        }
    } 
    while (message.empty());  
    
    //Creating commit
    string saveContent = "Tree: " + treeHash + "\n"
                         "Parent Commit: " + parentCommit + "\n"
                         "Author name: " + committer + "\n"
                         "Timestamp: " + timeStamp + "\n\n"
                         + message;                                                                      

                         
    string saveHash = get_hash(saveContent);
    //time to create commit object
    create_blob_file(saveHash,saveContent); //store commit object
    //updating main
    ofstream main(mainFile, ios::trunc);
    main << saveHash;
    main.close();
    //updating history
    update_history(saveHash, saveContent);
}

//  +++++++++++++++++++++++++  RESET +++++++++++++++++++++++++++++++

void delete_files(vector<vector<string>> treeTableVector) {

    vector<vector<string>> indexTableVector = index_to_vector();
    bool shouldKeep = false;
    string fileName;

    // Check if the current file EXISTS in tree file

    for(int i=0; i<indexTableVector.size(); i++) {
        fileName = indexTableVector[i][0];
        shouldKeep = false;

        for(int j=0; j<treeTableVector.size(); j++) {

            if(fileName == treeTableVector[j][0]) {
                shouldKeep = true;
                break;
            }
        }

        // If file does NOT EXIST in tree file DELETE it

        if(!shouldKeep) {
            fs::path filePath = fileName;

            if(exists(filePath)) {
                remove(filePath);
            }
        }
    }
    
}

void update_main(string parentCommit, fs::path mainFile) {
    
    if (!parentCommit.empty()) {
        ofstream printerHead(mainFile);
        printerHead << parentCommit;
        printerHead.close();
    }   
    else {
        return;
    }

}

string find_tree_hash(vector<string> args)  {

    //  Read the main file and get the hash of the latest commit

    fs::path mainFile = ".mygit/refs/heads/main";
    ifstream mainFileContent(mainFile);
    stringstream ss;
    ss << mainFileContent.rdbuf();
    string commitHash = ss.str();
    
    string commitFolderName = commitHash.substr(0,2); 
    string commitFileName = commitHash.substr(2);

    // Go to that specific commit file

    fs::path parentPath = ".mygit/objects";
    fs::path commitFolderPath = parentPath / commitFolderName;
    fs::path commitFilePath = commitFolderPath / commitFileName;

    ifstream commitObject(commitFilePath);
    string row;
    string parentRow, parentCommit;
    getline(commitObject, row); // Just here to move cursor to next line
    getline(commitObject, parentRow);
    commitObject.close();

    // Find the PARENT commit 

    if(parentRow.substr(0,6) == "Parent") {
        parentCommit = parentRow.substr(8);
    }

    // update refs/heads/main only when UNDO "RESET" is called

    if(args[1] == "undo") {
        update_main(parentCommit, mainFile);
    }
    if(parentCommit.empty()) {
        cerr << "Error: Cannot go back, this is the first commit!" << endl;
        return "";
    }

    // Get the hash of tree file from the PARENT commit

    string parentFolder = parentCommit.substr(0, 2);
    string parentFile = parentCommit.substr(2);
    fs::path parentCommitPath = fs::path(".mygit/objects") / parentFolder / parentFile;

    ifstream parentObject(parentCommitPath);
    string parentTreeRow;
    getline(parentObject, parentTreeRow);
    parentObject.close();

    // Return the PARENT tree hash

    return parentTreeRow.substr(6);
    
}

vector<vector<string>> tree_to_vector(string treeHash) {

    // Making the 2d string vector of tree file

    string treeFolderName = treeHash.substr(0,2); 
    string treeFileName = treeHash.substr(2);

    fs::path parentPath = ".mygit/objects";
    fs::path treeFolderPath = parentPath / treeFolderName;
    fs::path treeFilePath = treeFolderPath / treeFileName;


    vector<vector<string>> treeTableVector;
    ifstream treeFile (treeFilePath);
    string line;

    while (getline(treeFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        vector<string> row;
        string rowSegment;
        while (ss >> rowSegment) {
            row.push_back(rowSegment);
        }
        treeTableVector.push_back(row);
    }

    // Return the vector

    return treeTableVector;
}

void create_files_again(vector<vector<string>> treeTableVector) {

    // Run the loop for all files

    for (int i=0; i<treeTableVector.size(); i++) {

        fs::path fileName = treeTableVector[i][0];
        string blobHash = treeTableVector[i][1];

        string blobFolderName = blobHash.substr(0,2); 
        string blobFileName = blobHash.substr(2);

        fs::path parentPath = ".mygit/objects";
        fs::path blobFolderPath = parentPath / blobFolderName;
        fs::path blobFilePath = blobFolderPath / blobFileName;
        fs::path newFilePath = "." / fileName;

        // Creating each file from scratch

        ofstream newFile(newFilePath);
        if (!newFile.is_open()) {
            cout << "Error! Couldn't create the file" << newFilePath << endl;
            return;
        }
        newFile << read_file_content(blobFilePath);
        newFile.close();
        
    }

}

void reset_command(vector<string> args) {

    // Find hash of that tree file (1st go to the latest commit then find the hash of tree)

    string treeHash = find_tree_hash(args);

    // Get the 2d string vector of tree file

    vector<vector<string>> treeTableVector;

    treeTableVector = tree_to_vector(treeHash);

    // Delete EXTRA files

    delete_files(treeTableVector);

    // Read the content from the hash wali file & Make a file of that exact name and write the content there

    create_files_again(treeTableVector);

    // Updating the INDEX file
    vector_to_index(treeTableVector);    

}

//  +++++++++++++++++++++++++  HELP  +++++++++++++++++++++++++++++++

void help_command() {
    cout << "\n      -->HELP COMMAND MENU\n\n";
    cout << left << setw(10) << "create: " << "Create an Empty New Repository." << endl;
    cout << left << setw(10) << "add: "    << "Add File Contents to the Index." << endl;
    cout << left << setw(10) << "save: "   << "Save Changes to the Repository." << endl;
    cout << left << setw(10) << "set: "    << "List the Username." << endl;
    cout << left << setw(10) << "history: " << "Show Previous Commits." << endl;
    cout << left << setw(10) << "undo: "   << "Return to the Previous Commit Erasing the Current." << endl;
    cout << left << setw(10) << "revert: "   << "Return to the Previous Commit in the Form of a New Commit." << endl << endl;
}

//  +++++++++++++++++++++++++ REVERT +++++++++++++++++++++++++++++++


void revert_command(vector<string> args) {

    // Find hash of that tree file (1st go to the latest commit then find the hash of tree)

    string treeHash = find_tree_hash(args);

    // Get the 2d string vector of tree file

    vector<vector<string>> treeTableVector;

    treeTableVector = tree_to_vector(treeHash);

    // Delete EXTRA files

    delete_files(treeTableVector);

    // Read the content from the hash wali file & Make a file of that exact name and write the content there

    create_files_again(treeTableVector);

    // Updating the INDEX file because commit uses it
    
    vector_to_index(treeTableVector);

    // Make a new commit for revert

    commit_command();

}