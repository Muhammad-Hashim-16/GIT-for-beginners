#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

void path_functions() {
    // Path and its functions. 
    fs::path currentPathFolder = fs::current_path();
    fs::path currentPathFile = "D:/CS Project/Practice/filesystem.cpp";
    cout << "I am currently at " << currentPathFolder.string() << endl;
    cout << "The path of my file is " << currentPathFile.string() << endl;
    cout << "The name of the file is " << currentPathFile.filename().string() << endl;
    cout << "The name of the file without extension is " << currentPathFile.stem() << endl;
    cout << "The name of the file extension is " <<  currentPathFile.extension() << endl;
    cout << "The parent path is " <<  currentPathFile.parent_path() << endl;
}

void check_file() {
    string filename;
    cout << "Enter the file name: ";
    cin >> filename; 
    fs::path path1 = filename;
    bool answer;
    // answer = fs::exists(path1);

    fs::path targetFolder = ".";

    for (const auto& entry : fs::recursive_directory_iterator(targetFolder)) {
        if (entry.path().filename() == path1) {
            answer = 1;
            break;
        }
    }

    if (answer == 1) cout << "The file you entered exists.";
    else cout << "The file you entered doesn't exist.";
    cout << endl;

    if (fs::is_regular_file(path1)) cout << "It is a file.";
    else if (fs::is_directory(path1)) cout << "It is a folder.";
}

void create_repo() {
    fs::path checkFolder = ".mygit";
    if (fs::exists(checkFolder)) {
        cout << "Repo already exists!";
    } else {
        fs::create_directory(".mygit");
        fs::create_directories(".mygit/objects");
        fs::create_directories(".mygit/refs");
        
        cout << "Repo initialized!";
    }
}

void show_status() {
    fs::path targetFolder = ".";
    for (const auto& entry : fs::recursive_directory_iterator(targetFolder)) {
        cout << entry.path().filename().string() << endl;
    }
}

void my_files() {
    vector<string> myFiles;
    string pathString;
    for (const auto& entry : fs::recursive_directory_iterator("D:\\CS Project\\Practice")) {
        if (entry.is_regular_file()) {
            pathString = entry.path().string();
            myFiles.push_back(pathString);
        }
    }
    int count = myFiles.size();
    // cout << "I found " << count << " files." << endl;
    for (int i=0; i<7; i++) {
        cout << myFiles[i];
        cout << "  (" << fs::file_size(myFiles[i]) << " bytes)" << endl;
    }
}

string read_file_content() {
    std::ifstream file("hello.txt");
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void create_blob_file(string filename, string content) {
    std::ofstream blobFile(filename);
    if (!blobFile.is_open()) {
        cout << "Error! Couldn't create the file" << filename << endl;
        return ;
    }
    blobFile << content;
    blobFile.close();
    cout << "The file " << filename << " created with " << filename.size() << " bytes.";
}
