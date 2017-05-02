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

  void PopulateBuffer ()
  {
    file_.read(&buffer_[0], buffer_size_);
  }

public:
  FileReader (const std::string& file_name)
  {
    file_.open(file_name, std::ifstream::in);
    buffer_.resize(buffer_size_);
  }
  ~FileReader ()
  {
    file_.close();
  }
};