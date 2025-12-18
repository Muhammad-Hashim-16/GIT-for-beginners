#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "sha1.h"

// I am making CONFIG command!!!

// _____________ Logic for Config _____________
// 1- Make a Config file in INIT stage.
// 2- Declare a function that takes the name from the Terminal and saves it in Config file.
// 3- This functon should return the string containing the name.
// 4- Use this function while making the commit file.
 

using namespace std;
namespace fs = std::filesystem;

void config(int argc, char* argv[]) {

}

int main (int argc, char* argv[]) {
    if (argv[1] == "config") {
        if (argv[2] == "user.name") {
            config(argc, argv);
        }
    }
}







