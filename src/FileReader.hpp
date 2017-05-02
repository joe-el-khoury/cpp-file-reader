#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>

class FileReader
{
private:
  std::ifstream file_;
  static constexpr unsigned buffer_size_ = (4096 * sizeof(char));
  std::vector<char> buffer_;

public:
  FileReader (const std::string& file_name)
  {
    file_.open(file_name, std::ifstream::in);
    buffer_.reserve(buffer_size_);
  }
};