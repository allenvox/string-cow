#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace csc {

class String {
private:
  struct StringValue {
    int refCount{1};
    char *data;
    size_t capacity;
    size_t size;

    explicit StringValue(const char *d = "", size_t cap = 0)
        : capacity(cap), size(strlen(d)) {
      capacity = std::max(cap, size + 1); // + null-terminator
      data = new char[capacity];
      strlcpy(data, d, capacity);
    }

    ~StringValue() { delete[] data; }

    void detach() {
      if (refCount > 1) {
        --refCount;
        char *newData = new char[size + 1];
        strlcpy(newData, data, size + 1);
        data = newData;
      }
    }

    void resize(size_t newSize, char filled = '\0') {
      if (newSize > capacity) {
        char *newData = new char[newSize + 1];
        strlcpy(newData, data, size);
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

  } * value;

public:
  // Constructor from const char*
  explicit String(const char *d = "") { value = new StringValue(d); }

  // Move constructor
  String(String &&rhs) noexcept : value{rhs.value} { rhs.value = nullptr; }

  // Move assignment
  String &operator=(String &&rhs) noexcept {
    if (this != &rhs) {
      delete value;      // Free cur object
      value = rhs.value; // Move data
      rhs.value = nullptr;
    }
    return *this;
  }

  // Copy constructor
  String(const String &rhs) : value(rhs.value) {
    ++value->refCount; // Just new reference, no allocators
  }

  // Copy assignment
  String &operator=(const String &rhs) {
    if (this != &rhs) {
      if (--value->refCount == 0) {
        delete value;
      }
      value = rhs.value;
      ++value->refCount;
    }
    return *this;
  }

  // Destructor
  ~String() {
    if (value != nullptr && --value->refCount == 0) {
      delete value;
    }
  }

  // Operator[]
  char &operator[](size_t index) {
    value->detach();
    return value->data[index];
  }

  // Const operator[]
  const char &operator[](size_t index) const { return value->data[index]; }

  // At
  char &at(size_t index) {
    if (index >= value->size) {
      throw std::out_of_range("Index out of range");
    }
    value->detach();
    return value->data[index];
  }

  // Const at
  const char &at(size_t index) const {
    if (index >= value->size) {
      throw std::out_of_range("Index out of range");
    }
    return value->data[index];
  }

  // Resize
  void resize(size_t newSize, char filled = '\0') {
    value->detach();
    value->resize(newSize, filled);
  }

  // Reserve
  void reserve(size_t newCap) {
    if (newCap > value->capacity) {
      value->detach();
      // Create new buf with new capacity, copy data
      char *newData = new char[newCap + 1];
      strlcpy(newData, value->data, newCap + 1);
      delete[] value->data;
      value->data = newData;
      value->capacity = newCap;
    }
  }

  // Push back
  void push_back(char c) {
    if (value->size == value->capacity) {
      reserve(value->size * 2);
    }
    value->data[value->size++] = c;
    value->data[value->size] = '\0';
  }

  // Insert
  void insert(size_t pos, const char *str) {
    if (pos > value->size) {
      throw std::out_of_range("Position out of range");
    }
    size_t len = strlen(str);
    if (len + value->size > value->capacity) {
      reserve(len + value->size);
    }
    value->detach();
    // Push existing elements to the right
    for (size_t i = value->size; i >= pos; --i) {
      value->data[i + len] = value->data[i];
    }
    // Copy new data in pos
    for (size_t i = 0; i < len; ++i) {
      value->data[pos + i] = str[i];
    }
    value->size += len;
    value->data[value->size] = '\0';
  }

  // Erase
  void erase(size_t pos, size_t len) {
    if (pos >= value->size) {
      throw std::out_of_range("Position out of range");
    }
    if (pos + len > value->size) {
      len = value->size - pos; // Correct len
    }
    value->detach();
    // Push elements from right to the beginning of str
    for (size_t i = pos; i + len < value->size; ++i) {
      value->data[i] = value->data[i + len];
    }
    value->size -= len;
    value->data[value->size] = '\0';
  }

  // Begin
  char *begin() {
    value->detach();
    return value->data;
  }

  // Const begin
  const char *begin() const { return value->data; }

  // End
  char *end() {
    value->detach();
    return value->data + value->size;
  }

  // Const end
  const char *end() const { return value->data + value->size; }

  // Empty
  bool empty() const { return value->size == 0; }

  // Size
  size_t size() const { return value->size; }

  // Capacity
  size_t capacity() const { return value->capacity; }

  // Shrink to fit
  void shrink_to_fit() {
    if (value->capacity > value->size) {
      value->detach();
      char *newData = new char[value->size + 1];
      strlcpy(newData, value->data, value->size + 1);
      delete[] value->data;
      value->data = newData;
      value->capacity = value->size;
    }
  }

  // C string
  const char *c_str() const { return value->data; }
};

} // namespace csc