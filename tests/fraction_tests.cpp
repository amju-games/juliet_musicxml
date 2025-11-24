#include "catch.hpp"
#include "fraction.h"

TEST_CASE("Fraction inequality", "fraction")
{ 
  using namespace juliet_musicxml;
  REQUIRE(fraction(2, 1) != fraction(2, 2)); // 2 != 1
  REQUIRE_FALSE(fraction(13, 13) != fraction(7, 7)); // 1 != 1?
}

TEST_CASE("Fraction equality", "fraction")
{
  using namespace juliet_musicxml;
  REQUIRE(fraction(0, 1) == fraction(0, 9)); // 0 == 0
  REQUIRE(fraction(1, 1) == fraction(9, 9)); // 1 == 1
  REQUIRE(fraction(2, 1) == fraction(8, 4)); // 2 == 2
  REQUIRE_FALSE(fraction(2, 1) == fraction(4, 8)); // 2 == .5?
  REQUIRE_FALSE(fraction(8, 4) == fraction(1, 2)); // 2 == .5?
}

TEST_CASE("Fraction operator<", "fraction")
{
  using namespace juliet_musicxml;
  REQUIRE(fraction(2, 1) < fraction(17, 5)); // 2 < 3.4
  REQUIRE(fraction(7, 1) < fraction(17, 2)); // 7 < 8.5
  REQUIRE(fraction(7, 3) < fraction(17, 3)); // 2.333 < 5.666
  REQUIRE(fraction(7, 8) < fraction(17, 8)); // .875 < 2.125
  REQUIRE_FALSE(fraction(0, 1) < fraction(0, 9)); // 0 < 0?
  REQUIRE_FALSE(fraction(1, 1) < fraction(9, 9)); // 1 < 1?
  REQUIRE_FALSE(fraction(2, 1) < fraction(4, 8)); // 2 < .5?
  REQUIRE_FALSE(fraction(2, 1) < fraction(2, 2)); // 2 < 1?
}

TEST_CASE("Fraction operator+", "fraction")
{
  using namespace juliet_musicxml;
  REQUIRE(fraction(1, 5) + fraction(1, 3) == fraction(8, 15));
  REQUIRE(fraction(1, 5) + fraction(2, 5) == fraction(3, 5));
}

TEST_CASE("Fraction operator-", "fraction")
{
  using namespace juliet_musicxml;
  REQUIRE(fraction(1, 3) - fraction(2, 7) == fraction(1, 21));
  REQUIRE(fraction(3, 7) - fraction(2, 5) == fraction(1, 35));
}

TEST_CASE("Fraction operator+=", "fraction")
{
  using namespace juliet_musicxml;

  {
    // 1/5 + 1/3 = 8/15, right?
    fraction f(1, 5);
    f += fraction(1, 3);
    REQUIRE(f == fraction(8, 15));
  }

  {
    // 1/5 + 2/5 = 3/5, (and not 15/25!)
    fraction f(1, 5);
    f += fraction(2, 5);
    REQUIRE(f == fraction(3, 5));
    REQUIRE(f.num == 3);
    REQUIRE(f.denom == 5);
  }
}

TEST_CASE("Fraction operator-=", "fraction")
{
  using namespace juliet_musicxml;

  {
    // 1/3 - 2/7 = 1/21 ?
    fraction f(1, 3);
    f -= fraction(2, 7);
    REQUIRE(f == fraction(1, 21));
  }

  {
    // 3/7 - 4/7 = -1/7
    fraction f(3, 7);
    f -= fraction(4, 7);
    REQUIRE(f == fraction(-1, 7));
    REQUIRE(f == fraction(1, -7));
    REQUIRE(f.num == -1);
    REQUIRE(f.denom == 7);
  }
}

