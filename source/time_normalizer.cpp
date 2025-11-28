#include <sstream>
#include "score.h"
#include "time_normalizer.h"

namespace juliet_musicxml
{
void time_normalizer::normalize_times(
  bar& b, int& current_ticks, int& current_divs, fraction& norm_time)
{
  for (const auto& e: b.events) 
  {
    e->normalize_time(current_ticks, current_divs, norm_time);
  } 
}

void time_normalizer::normalize_times(score& sc)
{
  // TODO ***
  // I don't think we should rely on the accumulated time to be correct 
  //  across multiple bars. At the very least we should have a second opinion
  //  and report any discrepancy. We can calculate the expected start time
  //  for each bar based on time sigs/divisions per bar.
  // ***

  int unique_id = 0; // every event gets a unique ID, in ascending order of 
    // being found in the source.

  // Iterate over the parts, calculating times within that part.
  for (auto& [part_id, bars] : sc.parts)
  {
    // Reset time to 0 for this part
    int current_ticks = 0;
    int current_divs = 1; // and reset divisions
    fraction norm_time; // and normalized time

    for (auto& b : bars)
    {
      // Iterate over all events, tracking accumulated time and accounting
      //  for forward and backup events.
      // Using unique_ptr throughout: fix times in place
      normalize_times(*b, current_ticks, current_divs, norm_time);

      // Add unique ID for each event - what's the best way to do this?
      for (auto& ev : b->events) { ev->set_id(unique_id++); }

      // sort events in this one bar; we won't need to sort the events 
      //  in the entire sequence of bars... riiight?
      sort_events(b->events); 
    }
  }
}

void time_normalizer::sort_events(event_vec& events)
{
  std::sort(events.begin(), events.end());
}

bool operator<(const p_event& a, const p_event& b) 
{
  // TODO Remove this if we only sort one bar at a time
  if (a->m_bar_number != b->m_bar_number)
  {
    return a->m_bar_number < b->m_bar_number;
  }
  if (a->m_normalized_start_time != b->m_normalized_start_time)
  {
    return a->m_normalized_start_time < b->m_normalized_start_time;
  }
  // For a mix of attribute and note/rest events, the attributes should
  //  come first.
  if (a->m_is_attribute && !b->m_is_attribute)
  {
    return true;
  }

  return a->m_id < b->m_id;
}
}
