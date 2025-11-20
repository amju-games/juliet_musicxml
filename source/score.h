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
  // We have to keep the bars for each part separate so when we calc start 
  //  times using backup and forward events, we do it per-part.

  part_map parts;
};
}

