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

namespace bjvm {
bool HasSuffix(std::string_view str, std::string_view suffix);
std::vector<uint8_t> ReadFile(const std::string& file);
std::vector<std::string> ListDirectory(const std::string& path, bool recursive);

// Credit: https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // bjvm

#endif //UTILITIES_H
