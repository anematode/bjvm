//
// Created by Cowpox on 8/11/24.
//

#ifndef STRING_INTERNER_H
#define STRING_INTERNER_H

#include "native/string.h"
#include <string_view>

namespace bjvm {
/**
 * Strings are globally (well, per-VM) interned, so that two strings with the same value are the same object.
 */
class StringInterner {
  native::String Intern(std::string_view data) {
  }
};

} // bjvm

#endif //STRING_INTERNER_H
