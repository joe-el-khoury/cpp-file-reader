#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>

class FileReader
{
private:
  std::ifstream file_;
  
  unsigned buffer_size_;
  static constexpr unsigned chunk_size = (1024 * sizeof(char));
  std::vector<char> buffer_;
  bool buffer_init_ = false;

  std::ifstream::pos_type GetFileSize ()
  {
    file_.seekg(0, file_.end);
    std::ifstream::pos_type ret = file_.tellg();
    file_.seekg(0, file_.beg);

    return ret;
  }

  // Caculates the buffer size based on the size of the file.
  unsigned GetBufferSize ()
  {
    auto file_size = GetFileSize();
    
    unsigned buffer_size = (file_size / chunk_size);
    if (buffer_size == 0) {
      buffer_size = file_size;
    }
    
    return buffer_size;
  }

  void PopulateBuffer ()
  {
    file_.read(&buffer_[0], buffer_size_);
    buffer_init_ = true;
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
    
    buffer_size_ = GetBufferSize();
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

  std::string ReadLine (char delimiter='\n')
  {
    if (!buffer_init_) {
      PopulateBuffer();
    }
    
    std::string ret;

    int curr_char_idx = 0;
    while (!Done()) {
      if (curr_char_idx == buffer_size_-1) {
        PopulateBuffer();
        curr_char_idx = 0;
      }
      
      char curr_char = buffer_[curr_char_idx];
      ret += curr_char;
      if (curr_char == delimiter) {
        return ret;
      }

      curr_char_idx++;
    }

    return ret;
  }

  bool Done () const
  {
    return file_.eof();
  }
};