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
  // Get new event_vec for the given bar, with normalized times for the events.
  event_vec get_timed_events(const bar&, int& current_ticks, int& current_divs, fraction& norm_time);

  // For a whole score
  event_vec get_timed_events(const score&);

  // Sort by time for assembling into verticals
  static void sort_events(event_vec& events);
};
}

