//
// Created by nikita on 7/6/20.
//
#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedGlobalDeclarationInspection"

#include <vector>
#include <iostream>

using std::vector;
using std::ostream;
using std::istream;

class TriangleBoolSquareMatrix {
  size_t n = 0;
  vector<int> data;
public:
  TriangleBoolSquareMatrix() = default;

  explicit TriangleBoolSquareMatrix(size_t n);

  void setDimension(size_t newN);

  void randomInit();

  [[nodiscard]] size_t getDimension() const;

  [[nodiscard]] int at(size_t i, size_t j) const;

  [[nodiscard]] int unsafeAt(size_t i, size_t j) const;

  int &at(size_t i, size_t j);

  int &unsafeAt(size_t i, size_t j);

  friend ostream &operator<<(ostream &out, const TriangleBoolSquareMatrix &matrix);

  ostream &printTriangle(ostream &out) const;

  ostream &printFull(ostream &out) const;

  ostream &printTriangleSubmatrix(ostream &out, const vector<size_t> &indexes) const;

  ostream &printFullSubmatrix(ostream &out, const vector<size_t> &indexes) const;

  void writeToStream(ostream &out) const;

  void readFromStream(istream &in);

  void writeToStreamTriangle(ostream &out) const;

  void readFromStreamTriangle(istream &in);

  void writeToStreamFull(ostream &out) const;

  void readFromStreamFull(istream &in);

  bool operator==(const TriangleBoolSquareMatrix &second) const;
};

#pragma clang diagnostic pop