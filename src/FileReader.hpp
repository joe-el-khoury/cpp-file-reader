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
  std::ifstream::pos_type file_size_ = -1;
  static constexpr unsigned chunk_size_ = (1024 * sizeof(char));
  
  std::vector<char> buffer_;
  // The buffer, naturally, will be paged. This keeps track of what page we're on.
  int curr_buffer_page_ = 0;
  // And this keeps track of where we are in the buffer.
  int curr_buffer_idx_ = 0;

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

  /**
   * Caculates the buffer size based on the size of the file.
   */
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
    // Erase the buffer, in order to not use leftover characters from the previous page's buffer.
    buffer_.erase(buffer_.begin(), buffer_.end());
    
    // Where to seek to in the file.
    int seek_to = curr_buffer_idx_ + (curr_buffer_page_ * chunk_size_);
    file_.seekg(seek_to, file_.beg);

    // Find out how much we're going to read into the buffer. We need this information to avoid
    // reading "too much", i.e. past the end of the file.
    int read_len = buffer_size_;
    if (seek_to + buffer_size_ - 1 >= GetFileSize()) {
      read_len = static_cast<unsigned>(GetFileSize()) - seek_to + 1;
    }

    // Now read into the buffer.
    file_.read(&buffer_[0], read_len);
  }
  
  /**
   * Gets the next character in the buffer, filling it if needed.
   */
  char GetNextCharInBuffer ()
  {
    if (buffer_.empty()) {
      curr_buffer_idx_  = 0;
      curr_buffer_page_ = 0;
      
      goto PopulateAndReturn;
    }

    {
      // Check if we need to buffer a new page from the file.
      // This happens when we're at the end of the buffer.
      bool should_get_new_page = (curr_buffer_idx_ == buffer_size_-1)
                              || (curr_buffer_idx_ == buffer_.size()-1);
      if (should_get_new_page) {
        curr_buffer_idx_ = 0;
        curr_buffer_page_++;
        
        goto PopulateAndReturn;
      }
    }

    curr_buffer_idx_++;

PopulateAndReturn:
    PopulateBuffer();
    return buffer_[curr_buffer_idx_];
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

  bool Done () const
  {
    return file_.eof();
  }
};