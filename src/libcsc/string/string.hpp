#pragma once

#include <cstddef>

namespace csc {

class String {
private:
  struct StringValue {
    int refCount{1};
    char *data;
    size_t capacity;
    size_t size;

    explicit StringValue(const char *d = "", size_t cap = 0);
    ~StringValue();

    void detach();
    void resize(size_t newSize, char filled = '\0');
  } *value;

public:
  explicit String(const char *d = "");
  String(String &&other) noexcept;
  String &operator=(String &&other) noexcept;
  String(const String &rhs);
  String &operator=(const String &rhs);
  ~String();

  char &operator[](size_t index);
  const char &operator[](size_t index) const;

  char &at(size_t index);
  const char &at(size_t index) const;

  void resize(size_t newSize, char filled = '\0');
  void reserve(size_t newCap);
  void push_back(char c);
  void insert(size_t pos, const char *str);
  void erase(size_t pos, size_t len);

  char *begin();
  const char *begin() const;

  char *end();
  const char *end() const;

  bool empty() const;
  size_t size() const;
  size_t capacity() const;
  void shrink_to_fit();
};

} // namespace csc