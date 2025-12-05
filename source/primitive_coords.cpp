#include <array>
#include <iostream>
#include "primitive_coords.h"

namespace juliet_musicxml
{
// Get the vertical spacing between the bottom line of stave i and the
//  top line of stave i+1, within the given part.
float get_stave_spacing(int part_index, int stave_num_zb)
{
  return 6; // TODO
}

// Get y-coord of top line of the given stave in the given part.
float get_stave_top(int part_index, int stave_num_zb)
{
  float y = 0;
  for (int i = 0; i < part_index; i++)
  {
    // Add up the heights of all preceding parts, including vertical
    //  distances between the parts.         
    // TODO
  }

  // Add height of each preceding stave in this part, and the space 
  //  between each adjacent stave pair.
  for (int i = 0; i < stave_num_zb; i++)
  {
    y += 4; // the height of each preceding stave -- even one-line staves
    y += get_stave_spacing(part_index, i); 
  }

  // If this is a one-line stave, we add 2 to the y-coord so we're on  
  //  the middle line of the notional 5-line space.
  // TODO

  return y;
}

float calc_note_y_pos(const note& n, clef_sign clef)
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

  float y = get_stave_top(n.m_part_index, n.m_stave);
  int index = std::tolower(n.m_pitch.m_step) - 'a';
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
  float octave = static_cast<float>(4 - n.m_pitch.m_octave) * 3.5f;
  y += distance_from_top_stave + octave;
std::cout << "Note: " << n.m_pitch.m_step <<  n.m_pitch.m_octave << " dist: " << distance_from_top_stave << " oct: " << octave << " y: " << y << std::endl;

  return y;
}
}

