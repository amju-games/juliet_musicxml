#pragma once

#include <map>
#include <string>
#include "bar.h"

namespace juliet_musicxml
{
struct part
{
  std::string m_part_name;
  bar_vec m_bars;
};

// Represents the raw score parts.
struct score 
{
  // Map part ID to sequence of bars. 
  using part_vec = std::vector<part>;
  part_vec m_parts;

  // Add bar for a part that has been added with add_new_part.
  void add_bar_for_part(int part_index, std::unique_ptr<bar>& b)
  {
    assert(part_index < m_parts.size());
    m_parts[part_index].m_bars.emplace_back(std::move(b));
  }

  // Add a new part
  // TODO: check if it's an existing part
  // Return part index for the part.
  int add_new_part(std::string part_name)
  {
    part p;
    p.m_part_name = part_name;
    m_parts.emplace_back(std::move(p));
    return static_cast<int>(m_parts.size()) - 1;
  }

  int add_new_part(std::string part_name, bar_vec& bars)
  {
    part p;
    p.m_part_name = part_name;
    p.m_bars = std::move(bars);
    m_parts.emplace_back(std::move(p));
    return static_cast<int>(m_parts.size() - 1);
  }
};
}

