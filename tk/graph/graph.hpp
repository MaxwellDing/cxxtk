#pragma once

#include <algorithm>
#include <vector>
#include <memory>
#include "gsl/gsl-lite.hpp"
#include "noncopy.hpp"

namespace tk {

class Node: private Noncopy {
public:
  explicit Node(const std::string& name): name_(name) {}
  virtual ~Node() = default;
  Node* down(int idx) { return down_[idx]; }
  bool link(Node* dn) {
    if (std::any_of(down_.begin(), down_.end(), [dn](Node* n) { return n == dn; })) {
      return false;
    }
    down_.emplace_back(dn);
    return true;
  }
  bool unlink(Node* dn) {
    auto match = std::find(down_.begin(), down_.end(), dn);
    if (match == down_.end()) return false;
    down_.erase(match);
    return true;
  }

private:
  std::vector<gsl::not_null<Node*>> down_;
  std::string name_;
};

class Graph: private Noncopy {
public:
  virtual ~Graph() = default;
  template <class N, typename... Args>
  Node* addNode(const std::string& name, Args&&... args) {
    Node* n = new N(name, std::forward<Args>(args)...);
    nodes_.emplace_back(n);
    return n;
  }

private:
  std::vector<std::unique_ptr<Node>> nodes_;
  std::vector<gsl::not_null<Node*>> start_;
};

}  // namespace tk
