#include "interleaver.h"
#include "verticalizer.h" // or keep separate?

namespace juliet_musicxml
{
event_vec interleaver::interleave_score_parts(juliet_musicxml::score& s)
{
  // 1. FLATTEN: Create one giant list for all raw events from all parts.
  event_vec all_raw_events;

  int part_index = 0; // until we refactor score
  // Iterate over each part in the score
  for (auto& [part_id, bar_list] : s.parts)
  {
    // Keep track of the absolute start time for the beginning of the *next* bar 
    //fraction current_bar_start_time = fraction(0, 1); 

    // Iterate over each bar in this part's bar_vec
    for (auto& bar_ptr : bar_list)
    {   
      for (auto& event_ptr : bar_ptr->events)
      {   
        event_ptr->m_part_index = part_index;
          
        // Move the event into our flat list
        all_raw_events.emplace_back(std::move(event_ptr));
      }   
    
      // After finishing a bar, update the start time for the *next* bar 
      //current_bar_start_time += bar_ptr->get_duration(); // Assumes bar has duration
    }   
    part_index++;
    // (bar_ptr->m_events is now empty, as is the bar_list after this loop)
  }

  // VERTICALIZE
  // At this point, all_raw_events contains every event from every part, 
  // with correct absolute timings, but they are unsorted and ungrouped.

  sort(all_raw_events);
  
  verticalizer v;
  v.group_verticals(all_raw_events); // This sorts, groups by time, and groups chords

  // This list now contains a time-sorted, composite list of verticals.
  return all_raw_events;
}
}

