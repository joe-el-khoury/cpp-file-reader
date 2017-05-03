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
    // Set the file's exception flags.
    file_.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      file_.open(file_name, std::ifstream::in);
    } catch (const std::ifstream::failure& e) {
      throw std::runtime_error("Error opening file.");
    }
    buffer_.resize(buffer_size_);
  }
  ~FileReader ()
  {
    try {
      file_.close();
    } catch (const std::ifstream::failure& e) {
      throw std::runtime_error("Error closing file.");
    }
  }

  std::string ReadLine ()
  {
    // Don't do anything if we're already done reading.
    if (Done()) {
      return std::string();
    }

    std::string ret;
    std::getline(file_, ret);

    return ret;
  }

  bool Done () const
  {
    return file_.eof();
  }
};