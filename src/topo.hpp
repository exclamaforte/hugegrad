#pragma once
#include "scalar.hpp"
#include <fmt/ranges.h>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename T>
using TopoType = std::vector<ScalarNS::Scalar<T>>;

template <typename T>
void topo_visit(ScalarNS::Scalar<T> node, TopoType<T> &t)
{
  if (node->seen == ScalarNS::SeenMark::PERM) {
    return;
  }
  if (node->seen == ScalarNS::SeenMark::TMP) {
    throw new std::runtime_error("Cycle detected in topological sort.");
  }
  fmt::print("tsra: {}\n", node);
  node->seen = ScalarNS::SeenMark::TMP;
  switch (node->op->get_type()) {
  case Operation::OpType::BINARY:
    topo_visit(node->child1, t);
    topo_visit(node->child2, t);
    break;
  case Operation::OpType::UNARY:
    topo_visit(node->child1, t);
    break;
  case Operation::OpType::NONE:
    break;
  }
  node->seen = ScalarNS::SeenMark::PERM;
  t.push_back(node);
}

// outputs is a collection of all compute nodes that are not used in further computation
template <typename T>
TopoType<T> topological_sort(std::initializer_list<ScalarNS::Scalar<T>> outputs)
{
  TopoType<T> ret;
  for (const auto& o : outputs) {
    fmt::print("arst: {}\n", o);
    topo_visit(o, ret);
  }
  return ret;
}

template <typename T>
void backpropagate(std::initializer_list<ScalarNS::Scalar<T>> outputs)
{
  auto sorted = topological_sort(outputs);
  // initialize all the outputs' gradient as 1
  for (const auto &i : outputs) {
    i->grad = 1;
  }
  fmt::print("arsttsra: {}\n", sorted);
  for (auto i = sorted.rbegin(); i != sorted.rend(); ++i) {
    fmt::print("tsraarst: {}\n", *i);
    (*i)->propagate_gradient();
  }
}

