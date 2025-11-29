#pragma once

#include "expected.hpp" // TODO <expected> when available
#include "event.h"

namespace juliet_musicxml
{
struct bar;
struct fraction;
struct score;

class time_normalizer
{
public:
  // Normalize times in events in bar
  // Public for testing -- put in 'struct internal'

  void normalize_times(bar&, int& current_ticks, int& current_divs, fraction& norm_time);

  // Normalize times in each bar in score
  void normalize_times(score&);
};
}

