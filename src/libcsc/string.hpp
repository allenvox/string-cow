#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
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

  } *value;

public:
  // Detach value pointer
  void detach() {
    if (value->refCount > 1) {
      auto *newValue = new StringValue(value->data, value->capacity);
      --value->refCount;
      value = newValue;
    }
  }

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
      // Decrement the reference count of the current string
      if (--value->refCount == 0) {
        delete value;
      }
      // Copy the value pointer from rhs
      value = rhs.value;
      // Increment the reference count of the new value
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
    detach();
    return value->data[index];
  }

  // Const operator[]
  const char &operator[](size_t index) const { return value->data[index]; }

  // At
  char &at(size_t index) {
    if (index >= value->size) {
      throw std::out_of_range("Index out of range");
    }
    detach();
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
    detach();
    value->resize(newSize, filled);
  }

  // Reserve
  void reserve(size_t newCap) {
    if (newCap > value->capacity) {
      detach();
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
    detach();
    if (value->size + 1 >= value->capacity) {
      reserve(value->size * 2 + 2);
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
    detach();
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
    detach();
    // Push elements from right to the beginning of str
    for (size_t i = pos; i + len < value->size; ++i) {
      value->data[i] = value->data[i + len];
    }
    value->size -= len;
    value->data[value->size] = '\0';
  }

  // Empty
  bool empty() const { return value->size == 0; }

  // Size
  size_t size() const { return value->size; }

  // Capacity
  size_t capacity() const { return value->capacity; }

  // Shrink to fit
  void shrink_to_fit() {
    if (value->capacity > value->size) {
      detach();
      char *newData = new char[value->size + 1];
      strlcpy(newData, value->data, value->size + 1);
      delete[] value->data;
      value->data = newData;
      value->capacity = value->size;
    }
  }

  // C string
  const char *c_str() const { return value->data; }

  // Iterator class
  class iterator {
  private:
    char *ptr;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = char;
    using pointer = char *;
    using reference = char &;

    explicit iterator(char *p) : ptr(p) {}

    // Dereferencing
    reference operator*() const { return *ptr; }

    // Increment
    iterator &operator++() {
      ++ptr;
      return *this;
    }

    // Post-increment
    const iterator operator++(int) {
      iterator temp = *this;
      ++ptr;
      return temp;
    }

    // Decrement
    iterator &operator--() {
      --ptr;
      return *this;
    }

    // Post-decrement
    const iterator operator--(int) {
      iterator temp = *this;
      --ptr;
      return temp;
    }

    // Compound assignment +=
    iterator &operator+=(difference_type n) {
      ptr += n;
      return *this;
    }

    // Compound assignment -=
    iterator &operator-=(difference_type n) {
      ptr -= n;
      return *this;
    }

    // Subscripting
    reference operator[](difference_type n) const { return *(ptr + n); }

    // Addition
    friend iterator operator+(iterator it, difference_type n) {
      return iterator(it.ptr + n);
    }

    // Subtraction
    friend iterator operator-(iterator it, difference_type n) {
      return iterator(it.ptr - n);
    }

    // Equality
    friend bool operator==(const iterator &lhs, const iterator &rhs) {
      return lhs.ptr == rhs.ptr;
    }

    // Inequality
    friend bool operator!=(const iterator &lhs, const iterator &rhs) {
      return !(lhs == rhs);
    }

    // Less than
    friend bool operator<(const iterator &lhs, const iterator &rhs) {
      return lhs.ptr < rhs.ptr;
    }

    // Greater than
    friend bool operator>(const iterator &lhs, const iterator &rhs) {
      return lhs.ptr > rhs.ptr;
    }

    // Less than or equal to
    friend bool operator<=(const iterator &lhs, const iterator &rhs) {
      return lhs.ptr <= rhs.ptr;
    }

    // Greater than or equal to
    friend bool operator>=(const iterator &lhs, const iterator &rhs) {
      return lhs.ptr >= rhs.ptr;
    }

    // Difference
    friend difference_type operator-(const iterator &lhs, const iterator &rhs) {
      return lhs.ptr - rhs.ptr;
    }
  };

  // Const iterator
  using const_iterator = const iterator;

  // Begin method
  iterator begin() {
    detach();
    return iterator(value->data);
  }

  // Const begin method
  const const_iterator begin() const { return const_iterator(value->data); }

  // End method
  iterator end() {
    detach();
    return iterator(value->data + value->size);
  }

  // Const end method
  const const_iterator end() const {
    return const_iterator(value->data + value->size);
  }
};

// Concatenation operator
inline String operator+(const String &lhs, const String &rhs) {
  String result(lhs);
  result.reserve(result.size() + rhs.size());
  for (char rh : rhs) {
    result.push_back(rh);
  }
  return result;
}

// Comparison operators
inline bool operator==(const String &lhs, const String &rhs) {
  return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

inline bool operator!=(const String &lhs, const String &rhs) {
  return !(lhs == rhs);
}

inline bool operator<(const String &lhs, const String &rhs) {
  return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

inline bool operator<=(const String &lhs, const String &rhs) {
  return std::strcmp(lhs.c_str(), rhs.c_str()) <= 0;
}

inline bool operator>(const String &lhs, const String &rhs) {
  return std::strcmp(lhs.c_str(), rhs.c_str()) > 0;
}

inline bool operator>=(const String &lhs, const String &rhs) {
  return std::strcmp(lhs.c_str(), rhs.c_str()) >= 0;
}

// Stream output operator
inline std::ostream &operator<<(std::ostream &os, const String &str) {
  os << str.c_str();
  return os;
}

} // namespace csc