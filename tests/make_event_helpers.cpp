#include "make_event_helpers.h"

// Simplify creating notes for tests
std::unique_ptr<juliet_musicxml::note> make_note(std::string_view step, int time, int voice)
{
  using namespace juliet_musicxml;
  note n;
  n.m_pitch.m_step = step[0];
  n.m_normalized_start_time = fraction(time, 1); 
  n.m_voice = voice;
  return std::make_unique<note>(n);
}

std::unique_ptr<juliet_musicxml::rest> make_rest(int time, int voice)
{
  using namespace juliet_musicxml;
  rest r;
  r.m_normalized_start_time = fraction(time, 1);
  r.m_voice = voice;
  return std::make_unique<rest>(r);
}

std::unique_ptr<juliet_musicxml::clef_event> make_clef(int time, std::string sign)
{
  using namespace juliet_musicxml;
  clef_event clef;
  constexpr int stave = 1; // doesn't matter
  constexpr int line = 0; // doesn't matter 
  clef.m_clef_map[stave] = clef_and_line { sign, line };  
  clef.m_normalized_start_time = fraction(time, 1);
  return std::make_unique<clef_event>(clef);
}

