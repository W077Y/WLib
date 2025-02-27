#include <wlib-StringSink.hpp>
#include <cstring>

namespace wlib
{
  namespace
  {
    class null_sink_t: public StringSink_Interface
    {
      bool operator()(char const*, std::size_t ) override { return true; }
    };
  }    // namespace

  StringSink_Interface& StringSink_Interface::get_null_sink()
  {
    static null_sink_t obj;
    return obj;
  }

  bool StringSink_Interface::operator()(char const* c_str) { 
    return this->operator()(c_str, std::strlen(c_str)); 
  }

  bool StringSink_Interface::operator()(std::string_view str) { return this->operator()(str.data(), str.length()); }
}    // namespace wlib
