//
// Created by nikita on 7/6/20.
//
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedGlobalDeclarationInspection"

#include "TriangleBoolSquareMatrix.h"
#include <random>

using std::endl;
using std::out_of_range;
using std::runtime_error;
using std::to_string;
using std::random_device;
using std::mt19937;
using std::bernoulli_distribution;

TriangleBoolSquareMatrix::TriangleBoolSquareMatrix(size_t n) : n(n), data(n * (n - 1) / 2) {}

int TriangleBoolSquareMatrix::at(size_t i, size_t j) const {
  if (i > 0 && i < n && j < i) {
    return data[i * (i - 1) / 2 + j];
  } else {
    throw out_of_range(to_string(i) + ", " + to_string(j) + " is out of range of triangle square matrix of " +
                       to_string(n) + " dimension");
  }
}

int TriangleBoolSquareMatrix::unsafeAt(size_t i, size_t j) const {
  return data[i * (i - 1) / 2 + j];
}

int &TriangleBoolSquareMatrix::at(size_t i, size_t j) {
  if (i > 0 && i < n && j < i) {
    return data[i * (i - 1) / 2 + j];
  } else {
    throw out_of_range(to_string(i) + ", " + to_string(j) + " is out of range of triangle square matrix of " +
                       to_string(n) + " dimension");
  }
}

int &TriangleBoolSquareMatrix::unsafeAt(size_t i, size_t j) {
  return data[i * (i - 1) / 2 + j];
}

ostream &operator<<(ostream &out, const TriangleBoolSquareMatrix &matrix) {
  return matrix.printTriangle(out);
}

ostream &TriangleBoolSquareMatrix::printTriangle(ostream &out) const {
  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      out << at(i, j) << " ";
    }
    out << endl;
  }
  return out;
}

ostream &TriangleBoolSquareMatrix::printFull(ostream &out) const {
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (i == j) {
        out << 0 << " ";
      } else if (j > i) {
        out << at(j, i) << " ";
      } else {
        out << at(i, j) << " ";
      }
    }
    out << endl;
  }

  return out;
}

ostream &TriangleBoolSquareMatrix::printFullSubmatrix(ostream &out, const vector<size_t> &indexes) const {
  for (size_t i = 0; i < indexes.size(); ++i) {
    for (size_t j = 0; j < indexes.size(); ++j) {
      if (i == j) {
        out << 0 << " ";
      } else if (j > i) {
        out << at(indexes[j], indexes[i]) << " ";
      } else {
        out << at(indexes[i], indexes[j]) << " ";
      }
    }
    out << endl;
  }

  return out;
}

ostream &TriangleBoolSquareMatrix::printTriangleSubmatrix(ostream &out, const vector<size_t> &indexes) const {
  for (size_t i = 1; i < indexes.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      out << at(indexes[i], indexes[j]) << " ";
    }
    out << endl;
  }

  return out;
}

void TriangleBoolSquareMatrix::writeToStream(ostream &out) const {
  out.write(reinterpret_cast<const char *>(&n), sizeof n);

  unsigned char c = 0;
  size_t num = 0;
  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (at(i, j)) {
        c += static_cast<unsigned char>(1u << num);
      }
      ++num;
      if (num == 8) {
        out.put(char(c));
        num = 0;
        c = 0;
      }
    }
  }

  out.put(char(c));
}

void TriangleBoolSquareMatrix::readFromStream(istream &in) {
  in.read(reinterpret_cast<char *>(&n), sizeof n);
  if (!in) {
    throw runtime_error("IO error while reading matrix from stream");
  }

  data.resize(n * (n - 1) / 2);

  auto c = static_cast<unsigned char>(in.get());
  size_t num = 0;
  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (!in) {
        throw runtime_error("IO error while reading matrix from stream");
      }
      at(i, j) = (c & 1u) != 0;

      c >>= 1u;
      ++num;
      if (num == 8) {
        c = static_cast<unsigned char>(in.get());
        num = 0;
      }
    }
  }
}

bool TriangleBoolSquareMatrix::operator==(const TriangleBoolSquareMatrix &second) const {
  return n == second.n && data == second.data;
}

void TriangleBoolSquareMatrix::writeToStreamTriangle(ostream &out) const {
  out << n << endl;
  printTriangle(out);
}

void TriangleBoolSquareMatrix::readFromStreamTriangle(istream &in) {
  in >> n;

  int read;
  data.resize(n * (n - 1) / 2);
  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      in >> read;
      at(i, j) = read != 0;
    }
  }
}

void TriangleBoolSquareMatrix::writeToStreamFull(ostream &out) const {
  out << n << endl;
  printFull(out);
}

void TriangleBoolSquareMatrix::readFromStreamFull(istream &in) {
  in >> n;

  int read;
  data.resize(n * (n - 1) / 2);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      in >> read;
      if (i > 0 && i > j) {
        at(i, j) = read != 0;
      }
    }
  }
}

size_t TriangleBoolSquareMatrix::getDimension() const {
  return n;
}

void TriangleBoolSquareMatrix::randomInit() {
  static mt19937 twisterEngine((random_device()()));
  static bernoulli_distribution distribution;

  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      at(i, j) = distribution(twisterEngine);
    }
  }
}

void TriangleBoolSquareMatrix::setDimension(size_t newN) {
  n = newN;
  data.resize(n * (n - 1) / 2);
}

#pragma clang diagnostic pop