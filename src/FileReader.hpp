#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>

class FileReader {
private:
  FILE* file_;
public:
  FileReader (const std::string& file_name)
  {
    file_ = fopen(file_name.c_str(), "r");
  }
};