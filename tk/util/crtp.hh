/*************************************************************************
 * Copyright (C) [2020] by MaxwellDing. All rights reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *************************************************************************/

#ifndef TK_UTIL_CRTP_HH_
#define TK_UTIL_CRTP_HH_

namespace tk {
namespace util {

template <typename T, template<typename> class crtp_type>
class crtp {
 public:
  constexpr T& underlying() noexcept {
    return static_cast<T&>(*this);
  }
  constexpr T const& underlying() const noexcept {
    return static_cast<T const&>(*this);
  }
};

}  // namespace util
}  // namespace tk

#endif  // TK_UTIL_CRTP_HH_