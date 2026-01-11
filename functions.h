#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>
#include <filesystem>

// +++++++++++++++++++++++++ INIT ++++++++++++++++++++++++++++
void init_command();

// +++++++++++++++++++++++++ ADD +++++++++++++++++++++++++++++
std::string get_hash(std::string content);
void create_blob_file(std::string stringHash, std::string content);
std::string read_file_content(std::filesystem::path filename);
std::vector<std::vector<std::string>> index_to_vector();
void vector_to_index(std::vector<std::vector<std::string>> indexTableVector);
bool check_for_repo();
void add_command(int argc, std::vector<std::string> args);

// +++++++++++++++++++++++++ CONFIG ++++++++++++++++++++++++++++
void config_command(int argc, std::vector<std::string> args);
std::string use_config();

// +++++++++++++++++++++++++ LOG +++++++++++++++++++++++++++++++
void update_history(std::string folderName, std::string fileName);
void display_history();

// +++++++++++++++++++++++++ COMMIT ++++++++++++++++++++++++++++
std::string read_file(const std::string& filename);
std::string save_content(const std::string& content);
std::string get_time_string();
void commit_command();

// +++++++++++++++++++++++++ RESET ++++++++++++++++++++++++++++
void delete_files();
std::string find_tree_hash(std::vector<std::string> args);
std::vector<std::vector<std::string>> tree_to_vector(std::string treeHash);
void create_files_again(std::vector<std::vector<std::string>> treeTableVector);
void reset_command(std::vector<std::string> args);

// +++++++++++++++++++++++++ HELP ++++++++++++++++++++++++++++++
void help_command();

// +++++++++++++++++++++++++ REVERT ++++++++++++++++++++++++++++++
void revert_command(std::vector<std::string> args);

#endif