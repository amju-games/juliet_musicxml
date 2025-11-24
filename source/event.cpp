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
  if (m_is_chord) 
  {   
    // Chord notes have the start time of the previous note. Given that 
    //  every note in a chord has the same duration, that's the same
    //  as subtracting this note's duration from the cumulative 
    //  normalized time.  ...riiight?
    start_time = norm_time - event_duration;
  }   
  else 
  {   
    // Non-chord notes start at the current time pointer.
    start_time = norm_time;
  
    // Only non-chord notes advance the time.
    ticks += m_duration;
    norm_time += event_duration;
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
  m_normalized_start_time = norm_time; // keep the normalized start time at the original point, just for info.
  ticks -= m_duration;
  norm_time -= fraction(m_duration, divs);
  // ...not at the new time. Riight?

  std::stringstream ss;
  ss << "BACKUP " << m_normalized_start_time << ", " << m_duration << " units, from " << ticks + m_duration << " to " << ticks;
  m_description = ss.str();
}

void forward::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  std::stringstream ss;
  ss << "FORWARD " << m_normalized_start_time << ", " << m_duration << " units, from " << ticks << " to " << ticks + m_duration;
  m_description = ss.str();

  m_normalized_start_time = norm_time;

  ticks += m_duration;
  norm_time += fraction(m_duration, divs);
}

void divisions::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  std::stringstream ss;
  ss << "Divisions: " << m_num_divisions;
  m_description = ss.str();
  m_normalized_start_time = norm_time;
  divs = m_num_divisions;
}

}

