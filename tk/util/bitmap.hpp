#ifndef UTIL_BITMAP_HPP_
#define UTIL_BITMAP_HPP_

#include "int_def.h"

namespace util {

inline u32_t bitmapSetCount(u32_t bitmap) {
  bitmap = bitmap - ((bitmap >> 1) & 0x55555555);
  bitmap = (bitmap & 0x33333333) + ((bitmap >> 2) & 0x33333333);
  return (((bitmap + (bitmap >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

static constexpr u8_t _quick_find_table[] = {
    0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 00 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 10 */
    5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 20 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 30 */
    6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 40 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 50 */
    5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 60 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 70 */
    7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 80 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 90 */
    5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* A0 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* B0 */
    6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* C0 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* D0 */
    5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* E0 */
    4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0   /* F0 */
};
inline u32_t bitmapGetFirstSet(u32_t bitmap)  {
  if (bitmap & 0xff) {
    return _quick_find_table[bitmap & 0xff];
  } else if (bitmap & 0xff00) {
    return _quick_find_table[(bitmap >> 8) & 0xff] + 8;
  } else if (bitmap & 0xff0000) {
    return _quick_find_table[(bitmap >> 16) & 0xff] + 16;
  } else if (bitmap & 0xFF000000) {
    return _quick_find_table[(bitmap >> 24) & 0xFF] + 24;
  } else {
    return sizeof(decltype(bitmap)) * 8;
  }
}

}  // namespace util

#endif  // UTIL_BITMAP_HPP_
