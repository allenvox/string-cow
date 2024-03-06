#include <libcsc/string.hpp>

#include <gtest/gtest.h>

#include <utility>

using namespace csc;

TEST(StringTest, Constructors) {
  String s1;
  EXPECT_TRUE(s1.empty());
  String s2("hello");
  EXPECT_EQ(s2.size(), 5);
}

TEST(StringTest, MoveOperations) {
  // Create the original strings
  String s1("test1");
  String s2("test2");

  // Move the strings
  String moved1(std::move(s1));
  String moved2 = std::move(s2);

  // Check the properties of the moved-to objects
  EXPECT_EQ(moved1.size(), 5);
  EXPECT_EQ(moved2.size(), 5);
}

TEST(StringTest, CopyOperations) {
  String s1("test1");
  const String &copy1(s1);
  EXPECT_EQ(copy1.size(), s1.size());
  String s2("test2");
  const String &copy2 = s2;
  EXPECT_EQ(copy2.size(), s2.size());
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

TEST(StringTest, SizeCapacityReserve) {
  String s1;
  s1.reserve(10);
  EXPECT_GE(s1.capacity(), 10);
  String s2("hello");
  EXPECT_EQ(s2.size(), 5);
  EXPECT_GE(s2.capacity(), 5);
}

TEST(StringTest, Empty) {
  String s;
  EXPECT_TRUE(s.empty());
  s.push_back('a');
  EXPECT_FALSE(s.empty());
}

TEST(StringTest, ShrinkToFit) {
  String s("hello");
  s.reserve(20);
  EXPECT_GE(s.capacity(), 20);
  s.shrink_to_fit();
  EXPECT_EQ(s.capacity(), 5);
}

TEST(StringTest, ResizeMethod) {
  String s1("hello");
  s1.resize(3);
  EXPECT_EQ(s1.size(), 3);
  std::cerr << s1.c_str() << '\n';
  String s2("h");
  s2.resize(5, 'i');
  EXPECT_EQ(s2.size(), 5);
  EXPECT_EQ(s2[4], 'i');
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

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
