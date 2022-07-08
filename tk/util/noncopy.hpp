#pragma once

namespace tk {

class Noncopy {
public:
  Noncopy() = default;
  Noncopy(const Noncopy&) = delete;
  Noncopy& operator=(const Noncopy&) = delete;
};

}  // namespace tk
