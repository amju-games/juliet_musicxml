#pragma once

#include <memory>
#include <vector>
#include "event.h"

namespace juliet_musicxml
{
struct bar 
{
  int m_part_index = 0;
  int m_bar_number = 1; // There can be a bar #1 in each part;
    // I.e. part_index + bar_number is the unique ID of this bar.

  void set_part_index(int part_index);

  event_vec m_events; 
};

using bar_vec = std::vector<std::unique_ptr<bar>>;
}

