#include "fraction.h"

namespace juliet_musicxml
{
  void fraction::normalize()
  {
    if (num % denom == 0)
    {   
      num /= denom;
      denom = 1;
    }
  }

  fraction& fraction::operator+=(const fraction& f)
  {
    if (denom == f.denom)
    {
      num += f.num;
    }
    else 
    {
      *this = fraction(num * f.denom + f.num * denom, denom * f.denom);
    }
    normalize();
    return *this;
  }

  fraction& fraction::operator-=(const fraction& f)
  {
    if (denom == f.denom)
    {
      num -= f.num;
    }
    else
    {
      *this = fraction(num * f.denom - f.num * denom, denom * f.denom);
    }
    normalize();
    return *this;
  }

std::ostream& operator<<(std::ostream& os, const fraction& f)
{
/*
  // Simplified fraction can obscure what's happening
  if (f.num % f.denom == 0)
  {
    os << "{" << f.num / f.denom << "}";
  }
  else
  {
    os << "{" << f.num << "/" << f.denom << "}";
  }
*/
  os << "{" << f.num << "/" << f.denom << "}";
  return os; 
}
}

