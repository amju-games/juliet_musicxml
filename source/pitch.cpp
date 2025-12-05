#include <array>
#include "pitch.h"

namespace juliet_musicxml
{
expected_midi_pitch pitch::calc_midi_pitch() const
{
  // Base midi pitch values for the seven notes (steps)
  constexpr std::array<int, 7> PITCHES = 
  {
    9 /* a */, 11 /* b */, 0 /* c */, 2 /* d */, 4 /* e */, 5 /* f */, 7 /* g */
  };
  char step = m_step;
  if (step >= 'a' && step <= 'g') 
  {
    step -= 'a';
  }
  else if (step >= 'A' && step <= 'G') 
  {
    step -= 'A';
  }
  else
  {
    return tl::unexpected(pitch_error::PITCH_BAD_STEP);
  }
  assert(step >= 0);
  assert(step < 7);
  int pitch = PITCHES[step];
  pitch += m_alter;
  pitch += 12 * (m_octave + 1);
  if (pitch < 0)
  {
    return tl::unexpected(pitch_error::PITCH_TOO_LOW);
  } 
  else if (pitch > 127)
  {
    return tl::unexpected(pitch_error::PITCH_TOO_HIGH);
  }
  return pitch;
}

std::string pitch::to_string() const
{
  std::string s;

  if ((m_step >= 'a' && m_step <= 'g') ||
      (m_step >= 'A' && m_step <= 'G'))
  {
    s = std::string(1, m_step);    
  }
  else
  {
    return "X"; // percussion? - no pitch
  }
  // Add note alterations - i.e. sharps, flats
  for (int n = -1; n >= m_alter; --n)
  {
    s += "b"; // flat symbol
  }
  for (int n = 1; n <= m_alter; n++)
  {
    s += "#";
  }
  s += std::to_string(m_octave);
  return s;
}
}

