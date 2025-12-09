#include <sstream>
#include "event.h"
#include "i_renderer.h"

namespace juliet_musicxml
{
void event::normalize_time(int& ticks, int& divisions, fraction& time)
{
  m_normalized_start_time = time;
}

float composite_event::get_width() const
{
  float w = 0;
  for (const auto& ch : m_children)
  {
    w = std::max(w, ch->get_width());
  }
  return w;
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

  m_normalized_start_time = start_time;
  m_normalized_duration = event_duration;
}

std::string note::get_description() const
{
  std::stringstream ss; 
  ss << "Note: P" << m_part_index << " "
     << m_pitch.to_string() 
     << (m_is_chord ? " ch" : "   ") 
     << "\tDur: " << m_normalized_duration 
     << " Dur type: " << m_duration_type
     << " Start at: " << m_normalized_start_time 
     << " Stave: " << m_stave
     << " Voice: " << m_voice
     << " Stem: " << m_stem.to_string();
  return ss.str();
}

float note::get_width() const
{
  return 1.f;
}

void note::render(i_renderer& r) const 
{
  r.render_note(*this);
}

void rest::normalize_time(int& ticks, int& divs, fraction& time)
{
  const fraction event_duration(m_duration, divs);
  // Rests always advance the time pointer.
  ticks += m_duration;

  m_normalized_start_time = time;
  time += event_duration;

  m_normalized_duration = event_duration;
}

std::string rest::get_description() const
{
  std::stringstream ss;
  ss << "Rest: P" << m_part_index << " "
     << "\tDur: " << m_normalized_duration
     << " Dur type: " << m_duration_type
     << " Start at: " << m_normalized_start_time 
     << " Stave: " << m_stave
     << " Voice: " << m_voice;
  return ss.str();
}

void rest::render(i_renderer& r) const 
{
  r.render_rest(*this);
}

float rest::get_width() const
{
  return 1.f;
}

void backup::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  m_normalized_start_time = norm_time; // keep the normalized start time at the original point, just for info.
  ticks -= m_duration;
  norm_time -= fraction(m_duration, divs);
  // ...not at the new time. Riight?
}

std::string backup::get_description() const
{
  std::stringstream ss;
  ss << "BACKUP " << m_normalized_start_time; 
   /// << ", " << m_duration << " units, from " << ticks + m_duration << " to " << ticks;
  return ss.str();
}

void forward::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  m_normalized_start_time = norm_time;
  ticks += m_duration;
  norm_time += fraction(m_duration, divs);
}

std::string forward::get_description() const
{
  std::stringstream ss;
  ss << "FORWARD " << m_normalized_start_time;
   /// << ", " << m_duration << " units, from " << ticks << " to " << ticks + m_duration;
  return ss.str();
}

void divisions::normalize_time(int& ticks, int& divs, fraction& norm_time)
{
  m_normalized_start_time = norm_time;
  divs = m_num_divisions;
}

std::string divisions::get_description() const
{
  std::stringstream ss;
  ss << "Divisions: " << m_normalized_start_time << " new divisions value: " << m_num_divisions;
  return ss.str();
}

std::string stem::to_string() const
{
  return m_direction == direction::STEM_UP ? "U" : 
    (m_direction == direction::STEM_DOWN ? "D" : 
    "-");
}

std::string time_sig_event::get_description() const
{
  std::stringstream ss;
  // don't normalize this fraction!
  ss << "Time sig: " << m_fraction.num << "/" << m_fraction.denom;
  return ss.str();
}

void time_sig_event::render(i_renderer& r) const 
{
  r.render_time_sig(*this);
}

float time_sig_event::get_width() const
{
  return 1.f;
}

std::string key_sig_event::get_description() const
{
  std::stringstream ss;
  ss << "Key sig: " << to_string(m_key_sig);
  return ss.str();
}

void key_sig_event::render(i_renderer& r) const 
{
  r.render_key_sig(*this);
}

float key_sig_event::get_width() const
{
  return 1.f;
}

// TODO Move this keysig stuff

// Add an int to an enum value, giving another enum value as result.
// Just a convenience.
template <typename ENUM_TYPE>
ENUM_TYPE enum_add(ENUM_TYPE e, int i)
{
  return static_cast<ENUM_TYPE>(static_cast<int>(e) + i); 
}

bool key_sig_event::set_from_num_fifths(int n)
{
  if (n == 0) return false;
  if (n > 0)
  {
    m_key_sig = enum_add(key_sig::KEYSIG_0_SHARP, n);
  }   
  else 
  {
    m_key_sig = enum_add(key_sig::KEYSIG_0_FLAT, -n);
  }  
  return true;
}

std::string clef_event::get_description() const
{
  std::stringstream ss;
  ss << "Clefs: ";
  for (const auto& [stave, clef] : m_clef_map)
  {
    ss << "(stave:" << stave << ": " << to_string(clef).value() << ") ";
  }
  return ss.str();
}

void clef_event::render(i_renderer& r) const 
{
  r.render_clef(*this);
}

float clef_event::get_width() const
{
  return 1.f;
}

std::string stave_event::get_description() const
{
  std::stringstream ss;
  ss << "STAVES for part " << m_part_index 
     << ": num staves: " << m_stave_info.m_num_staves 
     << " lines: " << m_stave_info.m_num_stave_lines;
  return ss.str();
}

void stave_event::render(i_renderer& r) const 
{
  r.render_stave(*this);
}

float bar_line_event::get_width() const 
{
  return 1.f; // including a bit of space on either side
}

std::string bar_line_event::get_description() const 
{
  std::stringstream ss;
  ss << "Bar line for part " << m_part_index << " (bar: " << m_bar_number << ")";
  return ss.str();
}

void bar_line_event::render(i_renderer& r) const 
{
  r.render_bar_line(*this);
}

void composite_event::render(i_renderer& r) const 
{
  for (const auto& child : m_children)
  {
    child->render(r); // Recursively render children
  }
}

void sort(event_vec& events)
{
  std::sort(events.begin(), events.end());
}

bool operator<(const p_event& a, const p_event& b)  
{
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

