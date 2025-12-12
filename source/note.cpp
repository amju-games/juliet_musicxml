#include <algorithm>
#include <array>
#include <sstream>
#include "note.h"
#include "i_renderer.h"
#include "primitive_coords.h"

namespace juliet_musicxml
{
void note::pre_render_ledger_lines(clef_sign clef) const
{
  float y = calc_note_y_pos(clef);
  // Top line of stave is y==0. So number of ledger lines above is 
  //  the whole number of negative steps above that line.
  m_ledger_lines.m_num_ledger_lines_above = std::max(0, static_cast<int>(-y)); 
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

float note::calc_note_y_pos(clef_sign clef) const
{
/*
Use step, not MIDI pitch, because of enharmonic notes.
We can use step + octave to calc offset from top of stave.
Steps map to a value, octave then adds multiples of 3.5.

Treble clef:
Note  MIDI    Pos from top line of stave
F5    77      0   --------
E5    76      .5  
D5    74      1   --------
C5    72      1.5 
B4    71      2   --------
A4    69      2.5 
G4    67      3   --------
F4    65      3.5 
E4    64      4   --------
D4    62      4.5 
C4    60      5     -0-  (mid C ledger line) 

Using Octave 4:
a -> 2.5 
b -> 2
c -> 5 
d -> 4.5 
e -> 4
f -> 3.5 
g -> 3

Bass clef:
Note  MIDI    Pos from top 
B4    71      -4   
A4    69      -3.5
G4    67      -3   
F4    65      -2.5
E4    64      -2   
D4    62      -1.5
C4    60      -1    -0-  (mid C ledger line)
B3    59      -.5
A3    57       0   --------
G3    55       .5
F3    53       1   --------
E3    52       1.5
D3    50       2   --------
C3    48       2.5
B2    47       3   --------
A2    45       3.5
G2    43       4   --------
F2    41       4.5

Using Octave 4:
a -> -3.5
b -> -4
c -> -1
d -> -1.5
e -> -2
f -> -2.5
g -> -3

*/

  float y = get_stave_top(m_part_index, m_stave);
  int index = std::tolower(m_pitch.m_step) - 'a';
  assert(index >= 0 && index < 7); // catch bad steps way earlier than this
  float distance_from_top_stave = 0;
  if (clef == clef_sign::CLEF_TREBLE)
  {
    // Distances from top line for steps C4..B4
    constexpr std::array<float, 7> TREBLE_DISTANCES =
    {
      2.5f /* a */, 2.f  /* b */, 5.f /* c */, 4.5f /* d */,
      4.f  /* e */, 3.5f /* f */, 3.f /* g */
    };
    distance_from_top_stave = TREBLE_DISTANCES[index];
  }
  else if (clef == clef_sign::CLEF_BASS)
  {
    // Distances from top line for steps C4..B4
    constexpr std::array<float, 7> BASS_DISTANCES =
    {
      -3.5f /* a */, -4.f  /* b */, -1.f /* c */, -1.5f /* d */,
      -2.f  /* e */, -2.5f /* f */, -3.f /* g */
    };
    distance_from_top_stave = BASS_DISTANCES[index];
  }
  float octave = static_cast<float>(4 - m_pitch.m_octave) * 3.5f;
  y += distance_from_top_stave + octave;

  return y;
}
}

