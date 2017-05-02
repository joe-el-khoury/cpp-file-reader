#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>

class FileReader {
private:
  FILE* file_;
  static constexpr buffer_size_ = (4096 * sizeof(char));
  std::vector<char> buffer_;
public:
  FileReader (const std::string& file_name)
  {
    file_ = fopen(file_name.c_str(), "r");
    buffer_.resize(buffer_size_);
  }
};