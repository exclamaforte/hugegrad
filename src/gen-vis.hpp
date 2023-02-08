#pragma once
#include "scalar.hpp"
#include <cstdlib>
#include <fstream>
#include <stdlib.h>
#include <string>

template <typename T>
void rec_helper(std::shared_ptr<Scalar::ScalarValue<T>> &val, void *parent,
                std::back_insert_iterator<std::string> c) {
  void *val_void = static_cast<void *>(val.get());
  fmt::format_to(c, "id{} [label=\"{}\"]\n", val_void, val);
  if (parent) {
    fmt::format_to(c, "id{} -> id{}\n", val_void, parent);
  }
  if (val->op->type != Operation::OpType::NONE) {
    void *op_id = &val->op;
    fmt::format_to(c, "id{} [label=\"{}\"]\n id{} -> id{}\n", op_id, *val->op,
                   op_id, val_void);
    if (val->child1) {
      rec_helper(val->child1, op_id, c);
    }
    if (val->child2) {
      rec_helper(val->child2, op_id, c);
    }
  }
}

template <typename T>
std::string gen_vis(std::shared_ptr<Scalar::ScalarValue<T>> &vis) {
  std::string result = "";
  rec_helper(vis, nullptr, std::back_inserter(result));
  return fmt::format("{}", result);
}

template <typename T>
void write_vis(std::shared_ptr<Scalar::ScalarValue<T>> &val) {
  std::ofstream myfile;
  fmt::print("opening file...\n");
  myfile.open("graphvis.dot", std::ios::in | std::ios::trunc);
  if (myfile.good()) {
    std::string v = gen_vis(val);
    fmt::print("writing graph...\n");
    myfile << "digraph G {" << v << "\n}\n";
    myfile.close();
    fmt::print("generating svg...\n");
    system("dot -Tsvg graphvis.dot > graphvis.svg && open graphvis.svg");
  }
  else { fmt::print("unable to open file.\n"); }
}
