#pragma once

#include <ostream>

namespace juliet_musicxml
{
// Represent normalized times without using floating point numbers 
//  (using floats or doubles would bring problems, e.g. with equality tests)
struct fraction
{
  fraction() = default;
  fraction(int n, int d) : num(n), denom(d) {}
  int num = 0;
  int denom = 1;

  // Reduce denom to 1 if num is a multiple
  void normalize();

  fraction& operator+=(const fraction& f);

  fraction& operator-=(const fraction& f);

  friend std::ostream& operator<<(std::ostream&, const fraction& f);
};

inline bool operator<(const fraction& f1, const fraction& f2)
{
  // f1.num / f1.denom < f2.num / f2.denom === f1.num*f2.denom < f2.num*f2.denom
  return (f1.num * f2.denom) < (f2.num * f1.denom);
}

inline bool operator==(const fraction& f1, const fraction& f2)
{
  return !(f1 < f2) && !(f2 < f1);
}

inline bool operator!=(const fraction& f1, const fraction& f2)
{
  return !(f1 == f2);
}
}

