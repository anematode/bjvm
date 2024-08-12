//
// Created by Cowpox on 8/10/24.
//

#ifndef BROWSER_JVM_BYTE_READER_H
#define BROWSER_JVM_BYTE_READER_H

#include <exception>
#include <algorithm>
#include <vector>
#include <cassert>
#include <cstdint>
#include <string>

namespace bjvm {
/**
 * Helper class for reading class files.
 */
class ByteReader {
  std::vector<uint8_t> m_bytes;
  // what we're currently reading -- for more informative ClassFormat- and VerifyErrors
  std::string m_current_component = "file";

  // Base from the original classfile start (in case we're reading a slice)
  int m_base = 0;
  int m_read = 0;

  template <int N>
  void ReadBytes(const char* reason, char* data) {
    if (m_read + N > m_bytes.size())
      throw std::runtime_error("Unexpected end of file while reading " + std::string(reason));
    // Reverse big endian -> little endian
    std::reverse_copy(m_bytes.begin() + m_read, m_bytes.begin() + m_read + N, data);
    m_read += N;
  }

public:
  explicit ByteReader(std::vector<uint8_t> bytes) : m_bytes(std::move(bytes)) {}

  void SetCurrentComponent(const std::string& component);

  std::vector<uint8_t> NextNBytes(int n, const char* reason = nullptr);

  /** Create a slice of the file starting at the current position. */
  ByteReader slice(const std::string& component, int bytes) const;

  uint8_t NextU8(const char* reason = nullptr);
  int8_t NextI8(const char* reason = nullptr);
  uint16_t NextU16(const char* reason = nullptr);
  int16_t NextI16(const char* reason = nullptr);
  uint32_t NextU32(const char* reason = nullptr);
  int32_t NextI32(const char* reason = nullptr);
  uint64_t NextU64(const char* reason = nullptr);
  int64_t NextI64(const char* reason = nullptr);
  double NextF64(const char* reason = nullptr);
  float NextF32(const char* reason = nullptr);

  int GetOriginalOffs() const;
  int GetOffs() const;

  bool Eof() const;
};
}

#endif //BROWSER_JVM_BYTE_READER_H
