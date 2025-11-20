#include <sstream>
#include "score.h"
#include "time_normalizer.h"

namespace juliet_musicxml
{
void process_event(
    const rest& cr,
    int& current_ticks, 
    int& current_divs,
    fraction& norm_time,
    event_vec& partEvents)
{
    rest r(cr);

    const fraction event_duration(r.m_duration, current_divs);
    std::string eventDescription;

    std::stringstream ss;
    ss << "Rest: " << r.m_type << " | Dur: " << event_duration;
    eventDescription = ss.str();
             
    // Rests always advance the time pointer.
    current_ticks += event_duration.num;  // so... is this used??
    norm_time += event_duration;

    r.m_normalized_start_time = norm_time;
    r.m_normalized_duration = event_duration;
    r.m_description = eventDescription;
    
    partEvents.push_back(r);
}

void process_event(
    const note& cn,
    int& current_ticks, 
    int& current_divs,
    fraction& norm_time,
    event_vec& partEvents) 
{
    note n(cn);

    const fraction event_duration(n.m_duration, current_divs);
    std::string eventDescription;
    fraction start_time;

    // Set normalized start time - different if this is a chord note.
    if (n.m_is_chord && !partEvents.empty()) 
    {
        // Chord notes inherit the start time of the previous event.
        // We use the start time of the last added event. 
        assert(!partEvents.empty());
        const auto& e = partEvents.back();
        if (std::holds_alternative<note>(e))
        {
            start_time = std::get<note>(e).m_normalized_start_time;
        }
    } 
    else 
    { 
        // Non-chord notes start at the current time pointer.
        start_time = norm_time;
    }        
    std::stringstream ss;
    ss << "Note: " << n.m_pitch.to_string() 
       << (n.m_is_chord ? " (CH)" : "     ") 
       << " | Dur: " << event_duration 
       << " Start at: " << start_time 
       << " Staff: " << n.m_staff
       << " Voice: " << n.m_voice
       << " Stem: " << (n.m_stem == note_stem::STEM_UP ? "U" : (n.m_stem == note_stem::STEM_DOWN ? "D" : "-"));
    eventDescription = ss.str();

    // Only non-chord notes advance the time.
    if (!n.m_is_chord) {
        current_ticks += n.m_duration;
        norm_time += event_duration;
    }

    n.m_normalized_start_time = start_time;
    n.m_normalized_duration = event_duration;
    n.m_description = eventDescription;
    
    partEvents.push_back(n);
}

void process_event(
    const backup& cb,
    int& current_ticks,
    int& current_divs,
    fraction& norm_time,
    event_vec& partEvents) 
{
    backup b(cb);

    current_ticks -= b.m_duration;
    norm_time -= fraction(b.m_duration, current_divs);

    // Add a marker event for clarity
    std::stringstream ss;
    ss << "BACKUP " << b.m_duration << " units. Pointer at: " << current_ticks;

    b.m_normalized_start_time = norm_time;
    b.m_description = ss.str();
    
    partEvents.push_back(b);
}

void process_event(
    const forward& cf,
    int& current_ticks,
    int& current_divs,
    fraction& norm_time,
    event_vec& partEvents
) {
    forward f(cf);

    // Add a marker event for clarity (starts at current time, ends at new time)
    std::stringstream ss;
    ss << "FORWARD " << f.m_duration << " units. Pointer from " << current_ticks;

    f.m_normalized_start_time = norm_time;
    f.m_description = ss.str();

    partEvents.push_back(f);

    current_ticks += f.m_duration;
    norm_time += fraction(f.m_duration, current_divs);
}

void process_event(
    const divisions& cdivs,
    int& current_ticks,
    int& current_divs,
    fraction& norm_time,
    event_vec& partEvents)
{
    divisions divs(cdivs);

    // Marker event for clarity
    std::stringstream ss;
    ss << " Divisions: " << divs.m_num_divisions;

    divs.m_normalized_start_time = norm_time;
    divs.m_description = ss.str();

    partEvents.push_back(divs);

    current_divs = divs.m_num_divisions;
}

void process_event(
    const attributes& ca,
    int& current_ticks,
    int& current_divs,
    fraction& norm_time,
    event_vec& partEvents
) {
    attributes a(ca);

    std::stringstream ss;
    ss << "Attributes: Time " << a.m_beats << "/" << a.m_beat_type 
       << " Key: " << to_string(a.m_key_sig)
       << " Staves: " << a.m_num_staves
       << " Clefs: ";
    for (const auto& [staff, clef] : a.m_clefs)
    {
      ss << "(" << staff << ": " << clef.m_sign << " line " << clef.m_line << ") ";
    }

    a.m_normalized_start_time = norm_time;
    a.m_description = ss.str();

    partEvents.push_back(a);
}

void process_measure_event(
  const event_union& evu,
  int& current_ticks,
  int& current_divs,
  fraction& norm_time,
  event_vec& result_events) 
{
  // std::visit calls the correct process_event overload
  //  based on the 'active' type currently held in event_union.
  std::visit(
    [&](const auto& ev) 
    {
      process_event(ev, current_ticks, current_divs, norm_time, result_events);
    }, evu);
}

event_vec time_normalizer::get_timed_events(
  const bar& rb, int& current_ticks, int& current_divs, fraction& norm_time)
{
  event_vec result; 

  // Create a new event vec, consisting of only renderable events, with
  //  normalized times.
  for (const auto& ev: rb.events) 
  {
    process_measure_event(ev, current_ticks, current_divs, norm_time, result);
  } 

  return result;
}

event_vec time_normalizer::get_timed_events(const score& sc)
{
  // TODO ***
  // I don't think we should rely on the accumulated time to be correct 
  //  across multiple bars. At the very least we should have a second opinion
  //  and report any discrepancy. We can calculate the expected start time
  //  for each bar based on time sigs/divisions per bar.
  // ***

  event_vec result;
  ////int unique_id = 0; // every event gets a unique ID, in ascending order of 
    // being found in the source.

  // Iterate over the parts, calculating times within that part.
  for (const auto& [part_id, bars] : sc.parts)
  {
    // Reset time to 0 for this part
    int current_ticks = 0;
    int current_divs = 1; // and reset divisions
    fraction norm_time; // and normalized time

    for (auto& rb : bars)
    {
      // Iterate over all events, tracking accumulated time and accounting
      //  for forward and backup events.
      event_vec bar_events = get_timed_events(rb, current_ticks, current_divs, norm_time);

      // Add unique ID for each event - what's the best way to do this?
      //////for (auto& ev : bar_events) { ev.set_id(unique_id++); }

      // *** TODO ***
      // I don't know if we want to put all the events into one big vector.
      // I think we will end up sifting the events out into bar and part buckets!
      result.insert(result.end(), bar_events.begin(), bar_events.end());
    }
  }
  sort_events(result); // TODO We can just sort events within the events for one bar/part - no need to sort by bar and part then; just normalized_start_time and voice etc. No?? Oh, we do want all events happening at the same start time, across parts. Hmmm...
 
  return result;
}

void time_normalizer::sort_events(event_vec& events)
{
  // Get all events into order:
  // 1. bar
  // 1. NORMALIZED start time  
  // 2. part
  // 3. voice
  // 3. staff
  // 4. order in which the events appear in the source  - TODO
  std::sort(events.begin(), events.end());
}

bool operator<(const event& a, const event& b) 
{
  if (a.m_bar_number != b.m_bar_number)
  {
    return a.m_bar_number < b.m_bar_number;
  }
  if (a.m_normalized_start_time != b.m_normalized_start_time)
  {
    return a.m_normalized_start_time < b.m_normalized_start_time;
  }
  if (a.m_part != b.m_part)
  {
    return a.m_part < b.m_part; 
  }
//  if (a.m_voice != b.m_voice)
//  {
//    return a.m_voice < b.m_voice; 
//  }
  if (a.m_staff != b.m_staff)
  {
    return a.m_staff < b.m_staff; 
  }
  return a.m_id < b.m_id;
}
}
