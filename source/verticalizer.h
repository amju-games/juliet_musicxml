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
}

