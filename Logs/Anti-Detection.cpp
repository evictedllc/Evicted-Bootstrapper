#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
    srand(time(0));
    string programName = "MyProgram";

    // Generate a random number between 0 and 9999
    int randomNumber = rand() % 10000;

    // Add the random number to the program name
    programName += to_string(randomNumber);

    // Set the console window title to the new program name
    system(("title " + programName).c_str());

    // Rest of the program code goes here...

    return 0;
}
