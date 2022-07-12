#include "graph/graph.hpp"

int main() {
  tk::Graph g;
  tk::Node* n1 = g.addNode<tk::Node>("node 1");
  tk::Node* n2_1 = g.addNode<tk::Node>("node 2-1");
  tk::Node* n2_2 = g.addNode<tk::Node>("node 2-2");
  tk::Node* n3_1 = g.addNode<tk::Node>("node 3-1");
  tk::Node* n3_2 = g.addNode<tk::Node>("node 3-2");
  tk::Node* n3_3 = g.addNode<tk::Node>("node 3-3");
  tk::Node* n4 = g.addNode<tk::Node>("node 4");
  tk::Node* n5 = g.addNode<tk::Node>("node 5");

  n1->link(n2_1);
  n1->link(n2_2);
  n2_1->link(n3_1);
  n2_2->link(n3_2);
  n2_2->link(n3_3);
  n3_1->link(n4);
  n3_2->link(n4);
  n3_3->link(n4);
  n4->link(n5);

  n2_2->unlink(n3_2);
  n2_1->link(n3_2);

  return 0;
}
