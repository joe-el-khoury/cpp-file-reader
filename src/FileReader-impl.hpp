/**
 * Implementation of the methods in FileReader.hpp.
 */

//////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////

std::ifstream::pos_type FileReader::GetFileSize ()
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
unsigned FileReader::GetBufferSize ()
{
  auto file_size = GetFileSize();
  
  unsigned buffer_size = (file_size / chunk_size_);
  if (buffer_size == 0) {
    buffer_size = file_size;
  }
  
  return buffer_size;
}

void FileReader::PopulateBuffer ()
{
  // Erase the buffer, in order to not use leftover characters from the previous page's buffer.
  buffer_.erase(buffer_.begin(), buffer_.end());
  
  // Where to seek to in the file.
  int seek_to = curr_buffer_idx_ + (curr_buffer_page_ * chunk_size_);
  // Check if we're seeking past the end of the file.
  if (seek_to >= GetFileSize()) {
    done_ = true;
    return;
  }
  
  // Find out how much we're going to read into the buffer. We need this information to avoid
  // reading "too much", i.e. past the end of the file.
  int read_len = buffer_size_;
  if (seek_to + buffer_size_ - 1 >= GetFileSize()) {
    read_len = static_cast<int>(GetFileSize()) - seek_to + 1;
  }

  // Seek into the file.
  file_.seekg(seek_to, file_.beg);
  // Now resize the buffer and read into it.
  buffer_.resize(read_len);
  file_.read(&buffer_[0], read_len);
}

/**
 * Gets the next character in the buffer, filling it if needed.
 */
char FileReader::GetNextCharInBuffer ()
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

JustReturn:
  return buffer_[curr_buffer_idx_];

PopulateAndReturn:
  PopulateBuffer();
  if (Done()) {
    throw std::out_of_range("No more characters in the buffer");
  }
  return buffer_[curr_buffer_idx_];
}

//////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////

FileReader::FileReader (const std::string& file_name)
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
}

FileReader::~FileReader ()
{
  try {
    file_.close();
  } catch (const std::ifstream::failure& e) {
    throw std::runtime_error("Error closing file.");
  }
}

std::string FileReader::GetLine ()
{
  std::string ret;
  char c;
  try {
    while ((c = GetNextCharInBuffer()) != '\n') {
      ret += c;
    }
  } catch (const std::out_of_range& e) {
    return ret;
  }
  return ret;
}

std::string FileReader::GetUntil (char delimiter)
{
  std::string ret;
  char c;
  try {
    while ((c = GetNextCharInBuffer()) != delimiter) {
      ret += c;
    }
  } catch (const std::out_of_range& e) {
    return ret;
  }
  return ret;
}

char FileReader::GetChar ()
{
  try {
    return GetNextCharInBuffer();
  } catch (const std::out_of_range& e) {
    return '\0';
  }
}

/**
 * Gets n chars from the file. If there are less than n chars in the file, then a string
 * with less than n chars is returned.
 */
std::string FileReader::GetNChars (unsigned n)
{
  std::string ret;
  try {
    for (int i = 0; i < n; ++i) {
      ret += GetNextCharInBuffer();
    }
  } catch (const std::out_of_range& e) {
    return ret;
  }
  return ret;
}

bool FileReader::Done () const
{
  return done_;
}