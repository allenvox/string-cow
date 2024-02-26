#include <libcsc/string/string.hpp>

#include <gtest/gtest.h>

using namespace csc;

TEST(StringTest, DefaultConstructor) {
  String s;
  EXPECT_TRUE(s.empty());
}

TEST(StringTest, CharConstructor) {
  String s("hello");
  EXPECT_EQ(s.size(), 5);
}

TEST(StringTest, CopyConstructor) {
  String original("test");
  const String &copy(original);
  EXPECT_EQ(copy.size(), original.size());
}

TEST(StringTest, CopyAssignmentOperator) {
  String original("original");
  const String &copy = original;
  EXPECT_EQ(copy.size(), original.size());
}

TEST(StringTest, IndexOperator) {
  String s("hello");
  EXPECT_EQ(s[1], 'e');
}

TEST(StringTest, AtMethod) {
  String s("test");
  EXPECT_EQ(s.at(2), 's');
  EXPECT_THROW(s.at(5), std::out_of_range);
}

TEST(StringTest, ResizeMethod) {
  String s("hello");
  s.resize(3);
  EXPECT_EQ(s.size(), 3);
  s.resize(5, 'x');
  EXPECT_EQ(s.size(), 5);
  EXPECT_EQ(s[4], 'x');
}

TEST(StringTest, ReserveAndCapacity) {
  String s;
  s.reserve(10);
  EXPECT_GE(s.capacity(), 10);
}

TEST(StringTest, PushBack) {
  String s;
  s.push_back('a');
  EXPECT_EQ(s.size(), 1);
  EXPECT_EQ(s[0], 'a');
}

TEST(StringTest, Insert) {
  String s("hello");
  s.insert(5, " world");
  EXPECT_EQ(s.size(), 11);
  EXPECT_EQ(s[6], 'w');
}

TEST(StringTest, Erase) {
  String s("hello world");
  s.erase(5, 6);
  EXPECT_EQ(s.size(), 5);
}

TEST(StringTest, BeginEnd) {
  String s("test");
  EXPECT_EQ(*s.begin(), 't');
  EXPECT_EQ(*(s.end() - 1), 't');
}

TEST(StringTest, Empty) {
  String s;
  EXPECT_TRUE(s.empty());
  s.push_back('a');
  EXPECT_FALSE(s.empty());
}

TEST(StringTest, Size) {
  String s("hello");
  EXPECT_EQ(s.size(), 5);
}

TEST(StringTest, Capacity) {
  String s("hello");
  EXPECT_GE(s.capacity(), 5);
}

TEST(StringTest, ShrinkToFit) {
  String s("hello");
  s.reserve(20);
  EXPECT_GE(s.capacity(), 20);
  s.shrink_to_fit();
  EXPECT_EQ(s.capacity(), 5);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}