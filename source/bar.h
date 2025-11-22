#pragma once

#include <vector>
#include "event.h"

namespace juliet_musicxml
{
struct bar 
{
  std::string part_id; // ID of part in which this bar lives
  int bar_number = 1; // There can be a bar #1 in each part;
    // I.e. part_id + bar_number is the unique ID of this bar.

  event_vec events; 
};

using bar_vec = std::vector<bar>;
}

