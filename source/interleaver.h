#pragma once

#include "score.h"

namespace juliet_musicxml
{
class interleaver
{
public:
  // Interleave the events in the given score, moving them to the returned vec.
  static event_vec interleave_and_create_verticals(score& s);
};
}

