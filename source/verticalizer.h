#pragma once

#include "vertical.h"

namespace juliet_musicxml
{
struct score;

class verticalizer
{
public:
  void group_verticals(event_vec& events);

  void group_verticals(score& s);
};

namespace internal
{
std::unique_ptr<vertical> make_vertical(event_vec::iterator first, event_vec::iterator last);
}
}

