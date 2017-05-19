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

  bool done_ = false;

  std::ifstream::pos_type GetFileSize ();
  unsigned GetBufferSize ();

  void PopulateBuffer ();
  char GetNextCharInBuffer ();

public:
  FileReader (const std::string&);
  ~FileReader ();

  std::string GetLine ();
  std::string GetUntil (char);
  char GetChar ();
  std::string GetNChars (unsigned);
  
  bool Done () const;
};

#include "FileReader-impl.hpp"