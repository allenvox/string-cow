#include <libcsc/string/string.hpp>

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace csc {

String::StringValue::StringValue(const char *d, size_t cap)
    : capacity(cap), size(strlen(d)) {
  capacity = std::max(cap, size + 1); // + null-terminator
  data = new char[capacity];
  strlcpy(data, d, capacity);
}

String::StringValue::~StringValue() { delete[] data; }

void String::StringValue::detach() {
  if (refCount > 1) {
    --refCount;
    char *newData = new char[size + 1];
    strlcpy(newData, data, size + 1);
    data = newData;
  }
}

void String::StringValue::resize(size_t newSize, char filled) {
  if (newSize > capacity) {
    char *newData = new char[newSize + 1];
    strlcpy(newData, data, size + 1);
    delete[] data;
    data = newData;
    for (size_t i = size; i < newSize; ++i) {
      newData[i] = filled;
    }
    capacity = newSize;
  } else if (newSize < size) {
    for (size_t i = newSize; i < size; ++i) {
      data[i] = filled; // fill to new size
    }
  }
  size = newSize;
  data[size] = '\0';
}

// Move constructor
String::String(String &&other) noexcept : value(other.value) {
  other.value = nullptr;
}

// Move assignment
String &String::operator=(String &&other) noexcept {
  if (this != &other) {
    delete value;         // free cur object
    value = other.value;  // move data
    other.value = nullptr;
  }
  return *this;
}

String::String(const char *d) { value = new StringValue(d); }

String::String(const String &rhs) : value(rhs.value) {
  ++value->refCount; // new reference
}

String::~String() {
  if (--value->refCount == 0) {
    delete value;
  }
}

String &String::operator=(const String &rhs) {
  if (this != &rhs) {
    if (--value->refCount == 0) {
      delete value;
    }
    value = rhs.value;
    ++value->refCount;
  }
  return *this;
}

char &String::operator[](size_t index) {
  value->detach();
  return value->data[index];
}

const char &String::operator[](size_t index) const {
  return value->data[index];
}

char &String::at(size_t index) {
  if (index >= value->size) {
    throw std::out_of_range("Index out of range");
  }
  value->detach();
  return value->data[index];
}

const char &String::at(size_t index) const {
  if (index >= value->size) {
    throw std::out_of_range("Index out of range");
  }
  return value->data[index];
}

void String::resize(size_t newSize, char filled) {
  value->detach();
  value->resize(newSize, filled);
}

void String::reserve(size_t newCap) {
  if (newCap > value->capacity) {
    value->detach();
    // create new buf with new capacity, copy data
    char *newData = new char[newCap + 1];
    strlcpy(newData, value->data, newCap + 1);
    delete[] value->data;
    value->data = newData;
    value->capacity = newCap;
  }
}

void String::push_back(char c) {
  if (value->size == value->capacity) {
    reserve(value->size + 1);
  }
  value->data[value->size++] = c;
  value->data[value->size] = '\0';
}

char *String::begin() {
  value->detach();
  return value->data;
}

const char *String::begin() const { return value->data; }

char *String::end() {
  value->detach();
  return value->data + value->size;
}

const char *String::end() const { return value->data + value->size; }

bool String::empty() const { return value->size == 0; }

size_t String::size() const { return value->size; }

size_t String::capacity() const { return value->capacity; }

void String::shrink_to_fit() {
  if (value->capacity > value->size) {
    value->detach();
    char *newData = new char[value->size + 1];
    strlcpy(newData, value->data, value->size + 1);
    delete[] value->data;
    value->data = newData;
    value->capacity = value->size;
  }
}

void String::insert(size_t pos, const char *str) {
  if (pos > value->size) {
    throw std::out_of_range("Position out of range");
  }

  size_t len = strlen(str);
  if (len + value->size > value->capacity) {
    reserve(len + value->size);
  }

  value->detach();
  // push existing elements to the right
  for (size_t i = value->size; i >= pos; --i) {
    value->data[i + len] = value->data[i];
  }
  // copy new data in pos
  for (size_t i = 0; i < len; ++i) {
    value->data[pos + i] = str[i];
  }
  value->size += len;
  value->data[value->size] = '\0';
}

void String::erase(size_t pos, size_t len) {
  if (pos >= value->size) {
    throw std::out_of_range("Position out of range");
  }

  if (pos + len > value->size) {
    len = value->size - pos; // correct len
  }

  value->detach();
  // push elements from right to the beginning of str
  for (size_t i = pos; i + len < value->size; ++i) {
    value->data[i] = value->data[i + len];
  }
  value->size -= len;
  value->data[value->size] = '\0';
}

} // namespace csc