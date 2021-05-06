// some tricks to help accomplish some c++11 stuff in c++98
#pragma once

#include <limits>
#include <string>
#include <sstream>

// replacement for std::is_integral
template <typename T>
struct is_integral
{
  static const bool value;
};

template <typename T>
const bool is_integral<T>::value = std::numeric_limits<T>::is_integer;

namespace fakestd {
  // fake to_string
  template <typename T>
  std::string to_string(const T &thing) {
    std::ostringstream ss;
    ss << thing;
    return ss.str();
  }
}