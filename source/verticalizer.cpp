#include <sstream>
#include "verticalizer.h"

namespace juliet_musicxml
{
void verticalizer::group_verticals(event_vec& events)
{
  // TODO Assume event_vec is sorted -- or don't, and sort it here?

  // Find range of elements with same start time. Move them into a vertical and replace the range with this vertical. So now vertical is a Composite.


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

