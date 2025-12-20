#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>
#include <filesystem>

// --- Main Commands (Called by main.cpp) ---

void init_command();

void add_command(int argc, char* argv[]);

void config_command(int argc, std::vector<std::string> args);

void commit_command();

void display_history();


// --- Helper Functions (Used internally) ---

std::string get_hash(std::string content);

void create_blob_file(std::string stringHash, std::string content);

void update_history(std::string folderName, std::string fileName);

bool check_for_repo();
std::string read_file(const std::string& filename);
std::string get_time_string();

#endif