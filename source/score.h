#pragma once

#include <map>
#include <string>
#include "bar.h"

namespace juliet_musicxml
{
// Parts in a score: each part ID maps to a sequence of bars.
// Using vector so we can index into parts.
using part = std::pair<std::string, bar_vec>;
using part_vec = std::vector<part>;

// Represents the raw score parts.
struct score 
{
  // Map part ID to sequence of bars. 
  part_vec m_parts;

  part& add_new_part(std::string part_name, bar_vec& bars)
  {
    m_parts.emplace_back(std::make_pair(part_name, std::move(bars)));
    return m_parts.back();
  }

  part& add_new_part(std::string part_name)
  {
    m_parts.emplace_back(std::make_pair(part_name, bar_vec()));
    return m_parts.back();
  }
};
}

