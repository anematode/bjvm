//
// Created by Cowpox on 8/11/24.
//

#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <string_view>
#include <vector>
#include <iostream>

#define BJVM_DEBUG(arg) do { std::cout << __FILE__ << ":" << __LINE__ << ": " << (arg) << "\n"; } while (0)
#define BJVM_UNREACHABLE() do { std::cerr << __FILE__ << ":" << __LINE__ << ": Unreachable code reached\n"; std::abort(); } while (0)
#define BJVM_CHECKED_VEC(vec, idx) \
  ({ auto&& _vec = (vec); auto&& _idx = (idx); if (_idx >= _vec.size() || _idx < 0) { BJVM_DEBUG("Index out of bounds"); std::abort(); } _vec[_idx]; })
#define BJVM_CHECKED_POP(vec) \
  ([&] { auto&& _vec = (vec); if (_vec.empty()) { BJVM_DEBUG("Stack underflow"); std::abort(); } auto _top = _vec.back(); _vec.pop_back(); return _top; })()

namespace bjvm {
bool HasSuffix(std::string_view str, std::string_view suffix);
std::vector<uint8_t> ReadFile(const std::string& file);
std::vector<std::string> ListDirectory(const std::string& path, bool recursive);

// All things should be reinterpret_cast-ed to this type
using jvalue = uint64_t;

template <typename T>
jvalue ToJValue(T value) {
  jvalue result = 0;
  std::memcpy(&result, &value, sizeof(T));
  return result;
}

template <typename T>
T FromJValue(jvalue value) {
  T result;
  std::memcpy(&result, &value, sizeof(T));
  return result;
}

// Credit: https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // bjvm

#endif //UTILITIES_H
