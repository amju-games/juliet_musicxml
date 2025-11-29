#pragma once

#include "vertical.h"

namespace juliet_musicxml
{
struct score;

class verticalizer
{
public:
  static void group_verticals(event_vec& events);
};
}

