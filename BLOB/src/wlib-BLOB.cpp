#include <wlib-BLOB.hpp>

//
#include <stdexcept>

namespace wlib::blob
{
  void MemoryBlob::handle_overwrite_exception() { throw std::out_of_range("not enouth room to insert object"); }
  void MemoryBlob::handle_insert_exception() { throw std::out_of_range("not enouth room to insert object"); }
  void MemoryBlob::handle_remove_exception() { throw std::out_of_range("not enouth bytes left"); }
  void MemoryBlob::handle_read_exception() { throw std::out_of_range("not enouth bytes left to read"); }
  void MemoryBlob::handle_position_exception() { throw std::out_of_range("not enouth bytes left to read"); }
}    // namespace wlib::blob
