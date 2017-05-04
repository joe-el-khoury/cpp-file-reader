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
  char buffer_[buffer_size_];

  void PopulateBuffer ()
  {
    file_.read(&buffer_[0], buffer_size_);
  }

public:
  FileReader (const std::string& file_name)
  {
    // Set the file's exception flags.
    file_.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      file_.open(file_name, std::ifstream::in);
    } catch (const std::ifstream::failure& e) {
      throw std::runtime_error("Error opening file.");
    }
  }
  ~FileReader ()
  {
    try {
      file_.close();
    } catch (const std::ifstream::failure& e) {
      throw std::runtime_error("Error closing file.");
    }
  }

  std::string ReadLine (char delimiter='\n')
  {
    std::string ret;

    int curr_char_idx = 0;
    while (!Done()) {
      char curr_char = buffer_[curr_char_idx];
      ret += curr_char;
      if (curr_char == delimiter) {
        return ret;
      }

      if (curr_char_idx == buffer_size_-1) {
        PopulateBuffer();
        curr_char_idx = 0;
      
      } else {
        curr_char_idx++;
      }
    }

    return ret;
  }

  bool Done () const
  {
    return file_.eof();
  }
};