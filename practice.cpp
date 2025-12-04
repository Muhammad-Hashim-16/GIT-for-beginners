#include <iostream>
#include <filesystem>
#include "sha1.h"
#include <sstream>
#include <iomanip>

using namespace std;

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

int main () {
    string content;
    cout << "Type anything: ";
    cin >> content;
    cout << get_hash(content);
}