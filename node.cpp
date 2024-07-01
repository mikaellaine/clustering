#include "node.h"

void Node::addNeighbor(SP<Node> aNeighbor){
  mNeighbors.push_back(aNeighbor);
}
