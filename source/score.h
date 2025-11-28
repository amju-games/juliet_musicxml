#pragma once

#include <map>
#include <string>
#include "bar.h"

namespace juliet_musicxml
{
// Parts in a score: each part ID maps to a sequence of bars.
using part_map = std::map<std::string, bar_vec>; // unordered better here?

// Represents the raw score parts.
struct score 
{
  // Map part ID to sequence of bars. 
  part_map parts;
};
}

