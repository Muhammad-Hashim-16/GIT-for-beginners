#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

string copy_the_file(fs::path filename) {
    cout << "Copying the file content..." << endl;

    ifstream copyFileContent(filename);
    if (!copyFileContent.is_open()) {
        return "";
    }
    stringstream buffer;
    buffer << copyFileContent.rdbuf();
    string content = buffer.str();

    string stem = filename.stem().string();
    string extension = filename.extension().string();
    fs::path parent = filename.parent_path();

    fs::path newFilename = stem + "(1)" + extension;

    ofstream copyFile(newFilename);
    if (!copyFile.is_open()) {
        cout << "Error! Couldn't create the file " << newFilename << endl;
        return "";
    }
    copyFile << content;
    copyFile.close();
    cout << "The file " << newFilename << " copied successfully.";
    return "";
}

int main () {
    fs::path filename;
    cout << "Enter the name of file: ";
    cin >> filename;
    cout << endl;
    if (fs::exists(filename)) {
        copy_the_file(filename);
    } else {
        cout << "The file doesn't exist.";
    }
}