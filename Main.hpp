#include <iostream>
#include "function_reader.h"

int main() {
    FunctionReader reader("my_function.cpp");
    std::string function_str = reader.get_function();
    std::cout << "Function: " << function_str << std::endl;
    return 0;
}


# returns
