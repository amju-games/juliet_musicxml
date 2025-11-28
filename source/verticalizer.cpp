#include <sstream>
#include "chord.h"
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

// Assumes that all events in the range [first, last) share the same start time.
std::unique_ptr<vertical> make_vertical(
    event_vec::iterator first, 
    event_vec::iterator last)
{
  using namespace juliet_musicxml;

  // 1. Get the start time (assuming the range is non-empty)
//    const auto& start_time = (*first)->m_normalized_start_time;

  // 2. Separate notes by voice and collect other events (rests, etc.)
  //    We need to move the ownership from the input range.
  std::map<int, event_vec> voice_groups;
  event_vec non_note_events;
    
  // Use std::make_move_iterator for efficient transfer of unique_ptr ownership
  for (auto it = std::make_move_iterator(first); it != std::make_move_iterator(last); ++it)
  {
    p_event e = std::move(*it); // e now holds the unique_ptr ownership
    
    if (auto note_ptr = dynamic_cast<note*>(e.get()))
    {
      // Group notes by voice ID (m_voice)
      voice_groups[note_ptr->m_voice].emplace_back(std::move(e));
    }
    else
    {
      // Handle rests and other non-chordifiable events
      non_note_events.emplace_back(std::move(e));
    }
  }

  // 3. Chordify: Process each voice group into chords or single notes.
  event_vec final_vertical_children;
  
  for (auto& [voice_id, notes_in_voice] : voice_groups)
  {
    if (notes_in_voice.size() > 1)
    {
      // Case: CHORD (V-02, V-04 Time 5, V-05)
      auto new_chord = std::make_unique<chord>(notes_in_voice.begin(), notes_in_voice.end());
      final_vertical_children.emplace_back(std::move(new_chord));
    }
    else 
    {
      // Case: SINGLE NOTE (V-01, V-04 Time 1 - if single voice)
      // If a voice has only one note, it's a single event.
      final_vertical_children.emplace_back(std::move(notes_in_voice[0]));
    }
  }

  // 4. Recombine: Add rests/non-note events.
  // Case: REST (V-03, V-04 Time 3)
  std::move(non_note_events.begin(), non_note_events.end(), 
            std::back_inserter(final_vertical_children));

  // 5. Create and return the final vertical object.
  auto final_vertical = std::make_unique<vertical>(final_vertical_children.begin(), final_vertical_children.end());
    
  return final_vertical;
}
} // internal

void verticalizer::group_verticals(score& sc)
{
  // Group verticals in each bar
  for (auto& [part_id, bars] : sc.parts)
  {
    for (auto& b : bars)
    {
      group_verticals(b->events);
    }
  }
}

void verticalizer::group_verticals(event_vec& events)
{
  // Assume event_vec is sorted

  // Find range of elements with same start time. Move them into a vertical 
  //  and replace the range with this vertical. (So vertical is a Composite.)

  internal::remove_non_renderable_events(events);

  auto first = events.begin();
  auto last = first; 
  while (first != events.end())
  {
    // Find a range of events with the same start time.
    // Attributes are immediately split into a separate vertical.
    ++last;
    if (last == events.end() ||
        (*last)->m_normalized_start_time != (*first)->m_normalized_start_time ||
        (*first)->m_is_attribute)
    {
      // Move the range into a new vertical, then replace the range with the vertical.
      std::unique_ptr<vertical> vert = internal::make_vertical(first, last);
      auto insert_point = events.erase(first, last);
      first = events.insert(insert_point, std::move(vert));
      ++first;
      last = first;
    }
  }
}
}

