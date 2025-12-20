#include <iostream>
#include <iomanip>
using namespace std;
void helpCommand()
{
    cout << "\nAVAILABLE COMMANDS\n\n";
    cout << left << setw(10) << "add"    << "add file contents to the index" << endl;
    cout << left << setw(10) << "create" << "create an empty new repository" << endl;
    cout << left << setw(10) << "history"<< "show previous commits" << endl;
    cout << left << setw(10) << "save"   << "save changes to the repository" << endl;
    cout << left << setw(10) << "set"    << "list the username" << endl;
    cout << left << setw(10) << "undo"   << "return to the previous commit" << endl;
}
int main()
{
    helpCommand();
}