#include <iostream>

#include "util/connect.hh"

class SignalClass {
 public:
  void EmitSignal(int a, const std::string& b) {
    emit sig_a(a, b);
  }
 signals:
  tk::util::SyncSignal<int, const std::string&> sig_a;
};

void SlotFunc(int a, const std::string& b) {
  std::cout << "a: " << a << '\n'
            << "b: " << b << std::endl;
}

void TestSigSlot() {
  SignalClass sig_c;
  connect(&sig_c, sig_a, SlotFunc);
  sig_c.EmitSignal(5, "test signal slot");
}

int main(int argc, char** argv) {
  TestSigSlot();
}