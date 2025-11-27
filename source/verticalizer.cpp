#include <sstream>
#include "score.h"
#include "verticalizer.h"

namespace juliet_musicxml
{
namespace internal
{
void remove_non_renderable_events(event_vec& events)
{
  events.erase(
    std::remove_if(events.begin(), events.end(), 
      [](const p_event& pe) { return !pe->m_is_renderable; }), 
    events.end());
}
}

void verticalizer::group_verticals(score& sc)
{
  // Group verticals in each bar
  for (auto& [part_id, bars] : sc.parts)
  {
    for (auto& b : bars)
    {
      group_verticals(b.events);
    }
  }
}

void verticalizer::group_verticals(event_vec& events)
{
  // Assume event_vec is sorted

  // Find range of elements with same start time. Move them into a vertical 
  //  and replace the range with this vertical. So vertical is a Composite.

  internal::remove_non_renderable_events(events);

  auto first = events.begin();
  auto last = first; 
  while (first != events.end())
  {
    // Find a range of events with the same start time
    ++last;
    if (last == events.end() ||
        (*last)->m_normalized_start_time != (*first)->m_normalized_start_time)
    {
      // Move the range into a new vertical, then replace the range with the vertical.
      //  (vertical ctor moves range)
      std::unique_ptr<vertical> vert = std::make_unique<vertical>(first, last); 
      events.erase(first, last);
      events.insert(first, std::move(vert));
      ++first;
      last = first;
    }
  }
}
}

