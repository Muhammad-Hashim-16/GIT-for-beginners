#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "sha1.h"

using namespace std;
namespace fs = std::filesystem;

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

int main () {
    vector<vector<string>> indexTableVector;
    indexTableVector = index_to_vector();
    for (int i=0; i<indexTableVector.size(); i++) {
        for (int j=0; j<indexTableVector[j].size(); j++) {
            cout << indexTableVector[i][j] << "  ";
        }
        cout << endl;
    }
}







