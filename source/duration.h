#pragma once

#include <ostream>
#include <string>
#include "expected.hpp" // TODO <expected>

namespace juliet_musicxml
{
// Map xml 'type' string for notes and rests to a duration enum.
enum class duration
{
  DUR_MAXIMA,
  DUR_LONG,
  DUR_BREVE,
  DUR_WHOLE,
  DUR_HALF,
  DUR_QUARTER,
  DUR_8TH,
  DUR_16TH,
  DUR_32ND,
  DUR_64TH,
  DUR_128TH,
  DUR_256TH,
  DUR_512TH,
  DUR_1024TH
};

tl::expected<duration, std::string> from_string(std::string xml_type);

std::ostream& operator<<(std::ostream& os, duration d);
}

