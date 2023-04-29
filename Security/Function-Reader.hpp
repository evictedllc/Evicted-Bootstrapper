#ifndef FUNCTION_READER_H
#define FUNCTION_READER_H

#include <string>
#include <fstream>
#include <streambuf>

class FunctionReader {
public:
    FunctionReader(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string str((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
            function_ = str;
            file.close();
        }
    }

    std::string get_function() const {
        return function_;
    }

private:
    std::string function_;
};

#endif
