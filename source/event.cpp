#include <sstream>
#include "event.h"

namespace juliet_musicxml
{
void event::normalize_time(int& ticks, int& divisions, fraction& time)
{
}

void attributes::normalize_time(int& ticks, int& divisions, fraction& norm_time)
{
  std::stringstream ss;
  ss << "Attributes: Time " << m_beats << "/" << m_beat_type
     << " Key: " << to_string(m_key_sig)
     << " Staves: " << m_num_staves
     << " Clefs: ";
  for (const auto& [staff, clef] : m_clefs)
  {
    ss << "(" << staff << ": " << clef.m_sign << " line " << clef.m_line << ") ";
  }
  m_description = ss.str();

  m_normalized_start_time = norm_time;
}

void note::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  const fraction event_duration(m_duration, divs);
  fraction start_time;

  // Set normalized start time - different if this is a chord note.
  if (m_is_chord) // TODO //  && !partEvents.empty()) 
  {   
    // Chord notes inherit the start time of the previous event.
    // We use the start time of the last added event. 
//    assert(!partEvents.empty());
//    const auto& e = partEvents.back();
//    if (std::holds_alternative<note>(e))
//    {   
//      start_time = std::get<note>(e).m_normalized_start_time;
//    }   
  }   
  else 
  {   
    // Non-chord notes start at the current time pointer.
    start_time = norm_time;
  }    
  std::stringstream ss; 
  ss << "Note: " << m_pitch.to_string() 
     << (m_is_chord ? " (CH)" : "     ")  
     << " | Dur: " << event_duration 
     << " Start at: " << start_time 
     << " Staff: " << m_staff
     << " Voice: " << m_voice
     << " Stem: " << (m_stem == note_stem::STEM_UP ? "U" : (m_stem == note_stem::STEM_DOWN ? "D" : "-"));    
  m_description = ss.str();

  // Only non-chord notes advance the time.
  if (!m_is_chord) 
  {
    ticks += m_duration;
    norm_time += event_duration;
  }

  m_normalized_start_time = start_time;
  m_normalized_duration = event_duration;
}

void rest::normalize_time(int& ticks, int& divs, fraction& time)
{
  const fraction event_duration(m_duration, divs);
  // Rests always advance the time pointer.
  ticks += m_duration;
  time += event_duration;

  std::stringstream ss;
  ss << "Rest: " << m_type << " | Dur: " << event_duration;
  m_description = ss.str();
  m_normalized_start_time = time;
  m_normalized_duration = event_duration;
}

void backup::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  ticks -= m_duration;
  norm_time -= fraction(m_duration, divs);

  std::stringstream ss;
  ss << "BACKUP " << m_duration << " units. Pointer at: " << ticks;

  m_normalized_start_time = norm_time;
  m_description = ss.str();
}

void forward::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  std::stringstream ss;
  ss << "FORWARD " << m_duration << " units. Pointer from " << ticks;
  m_description = ss.str();

  m_normalized_start_time = norm_time;

  ticks += m_duration;
  norm_time += fraction(m_duration, divs);
}

void divisions::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  std::stringstream ss;
  ss << " Divisions: " << m_num_divisions;
  m_description = ss.str();
  m_normalized_start_time = norm_time;
  divs = m_num_divisions;
}

}

