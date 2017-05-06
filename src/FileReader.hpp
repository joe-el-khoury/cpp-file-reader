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
  static constexpr unsigned chunk_size_ = (1024 * sizeof(char));
  std::ifstream::pos_type file_size_ = -1;
  
  std::vector<char> buffer_;
  bool buffer_init_ = false;
  int curr_buffer_idx_  = 0;

  std::ifstream::pos_type GetFileSize ()
  {
    if (file_size_ != -1) {
      return file_size_;
    }
    
    std::ifstream::pos_type curr_pos = file_.tellg();
    file_.seekg(0, file_.end);
    std::ifstream::pos_type ret = file_.tellg();
    file_.seekg(curr_pos, file_.beg);

    return ret;
  }

  // Caculates the buffer size based on the size of the file.
  unsigned GetBufferSize ()
  {
    auto file_size = GetFileSize();
    
    unsigned buffer_size = (file_size / chunk_size_);
    if (buffer_size == 0) {
      buffer_size = file_size;
    }
    
    return buffer_size;
  }

  void PopulateBuffer ()
  {
    file_.seekg(curr_buffer_idx_, file_.beg);
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
    
    file_size_   = GetFileSize();
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

    while (!Done()) {
      if (curr_buffer_idx_ == buffer_size_-1) {
        PopulateBuffer();
        curr_buffer_idx_ = 0;
      }
      
      char curr_char = buffer_[curr_buffer_idx_];
      ret += curr_char;
      if (curr_char == delimiter) {
        return ret;
      }

      curr_buffer_idx_++;
    }

    return ret;
  }

  bool Done () const
  {
    return file_.eof();
  }
};