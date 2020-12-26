#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include "util/cow_ptr.hh"

using tk::util::cow_ptr;
using tk::util::make_cow;

TEST(TK_Test, CowPtr) {
  cow_ptr<std::string> empty_ptr;
  EXPECT_FALSE(empty_ptr);
  cow_ptr<std::string> str = make_cow<std::string>("a test string");
  ASSERT_TRUE(str);
  cow_ptr<std::string> str_cp = str;
  const cow_ptr<std::string>& cstr = str;
  EXPECT_EQ(str, str_cp);
  EXPECT_EQ(str, cstr);

  // share on read
  std::cout << cstr.get() << std::endl;
  EXPECT_EQ(str, cstr);
  std::cout << str.get_const() << std::endl;
  EXPECT_EQ(str, cstr);
  std::cout << *cstr << std::endl;
  EXPECT_EQ(str, cstr);

  // copy on write
  std::cout << *str_cp << std::endl;
  EXPECT_NE(str.get_const(), str_cp.get_const());
  EXPECT_TRUE(str != str_cp);
  EXPECT_STREQ(str.get_const()->c_str(), str_cp.get_const()->c_str());
  cow_ptr<std::string> another_cp = str;
  another_cp.get();
  EXPECT_NE(str.get_const(), another_cp.get_const());

  // test operator <
  std::map<cow_ptr<std::string>, int> cow_map;
}
