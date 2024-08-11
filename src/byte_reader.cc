//
// Created by Cowpox on 8/10/24.
//
#include "byte_reader.h"

namespace bjvm {

std::vector<uint8_t> ByteReader::NextNBytes(int n, const char *reason) {
  std::vector<uint8_t> result;
  if (m_read + n > m_bytes.size())
    throw std::runtime_error("Unexpected end of " + m_current_component + " while reading " + reason);
  result.resize(n);
  memcpy(result.data(), m_bytes.data() + m_read, n);
  m_read += n;
  return result;
}

void ByteReader::SetCurrentComponent(const std::string &component) {
  m_current_component = component;
}

#define IMPL(name, ty) \
  ty ByteReader::name(const char *reason) { \
    ty result;               \
    ReadBytes<sizeof(result)>(reason, (char*) &result); \
    return result; \
  }

IMPL(NextU8, uint8_t) IMPL(NextU16, uint16_t) IMPL(NextU32, uint32_t) IMPL(NextU64, uint64_t)
IMPL(NextI8, int8_t) IMPL(NextI16, int16_t) IMPL(NextI32, int32_t) IMPL(NextI64, int64_t)
IMPL(NextF32, float) IMPL(NextF64, double)

int ByteReader::GetOriginalOffs() const {
  return m_read + m_base;
}

int ByteReader::GetOffs() const {
  return m_read;
}

ByteReader ByteReader::slice(const std::string &component, int bytes) const {
  if (m_read + bytes > m_bytes.size())
    throw std::runtime_error("Unexpected end of " + m_current_component + " while reading " + component);

  ByteReader reader ({m_bytes.begin() + m_read, m_bytes.begin() + m_read + bytes});
  reader.m_current_component = component;
  reader.m_base = m_base + m_read;
  return reader;
}

bool ByteReader::Eof() const {
  return m_read >= m_bytes.size();
}

}