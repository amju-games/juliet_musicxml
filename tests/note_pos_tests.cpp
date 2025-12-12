#include "catch.hpp"
#include "event.h"
#include "note.h"
#include "primitive_coords.h"

namespace juliet_musicxml
{
note make_note(char step, int octave)
{
  note n;
  n.m_pitch.m_step = step;
  n.m_pitch.m_octave = octave;
  return n;
}

float calc_note_y_pos(const note& n, clef_sign c)
{
  return n.calc_note_y_pos(c);
}
}

// Get the nearest int to f; but if f is not within 0.01 of f, assert.
// This is for checking float values are very close to an int value.
int near_int(float f)
{
  REQUIRE(std::abs(std::round(f) - f) < 0.01f);
  return std::round(f);
}

// Test calculation of y-coord for notes on the stave.
// The coordinate system is: the top line of the stave is 0.
// Positive y is downwards. The distance between two adjacent
//  stave lines is 1.0. 
TEST_CASE("note in correct pos, treble clef", "[note pos tests]")
{
  using namespace juliet_musicxml;

  // Position of C4 (middle C) from the top line of the stave 
  //  with a treble clef is 5 units. 
  // The top line of stave with treble clef is F5, where distance is zero.
  REQUIRE(near_int(calc_note_y_pos(make_note('a', 3), clef_sign::CLEF_TREBLE)) == 6);
  REQUIRE(near_int(calc_note_y_pos(make_note('c', 4), clef_sign::CLEF_TREBLE)) == 5);
  REQUIRE(near_int(calc_note_y_pos(make_note('e', 4), clef_sign::CLEF_TREBLE)) == 4);
  REQUIRE(near_int(calc_note_y_pos(make_note('g', 4), clef_sign::CLEF_TREBLE)) == 3);
  REQUIRE(near_int(calc_note_y_pos(make_note('b', 4), clef_sign::CLEF_TREBLE)) == 2);
  REQUIRE(near_int(calc_note_y_pos(make_note('d', 5), clef_sign::CLEF_TREBLE)) == 1);
  REQUIRE(near_int(calc_note_y_pos(make_note('f', 5), clef_sign::CLEF_TREBLE)) == 0);
  REQUIRE(near_int(calc_note_y_pos(make_note('a', 5), clef_sign::CLEF_TREBLE)) == -1);
  REQUIRE(near_int(calc_note_y_pos(make_note('c', 6), clef_sign::CLEF_TREBLE)) == -2);
}

TEST_CASE("note in correct pos, bass clef", "[note pos tests]")
{
  using namespace juliet_musicxml;

  // Position of C4 (middle C) from the top line of the stave 
  //  with a bass clef is -1 units. 
  REQUIRE(near_int(calc_note_y_pos(make_note('c', 2), clef_sign::CLEF_BASS)) == 6);
  REQUIRE(near_int(calc_note_y_pos(make_note('e', 2), clef_sign::CLEF_BASS)) == 5);
  REQUIRE(near_int(calc_note_y_pos(make_note('g', 2), clef_sign::CLEF_BASS)) == 4);
  REQUIRE(near_int(calc_note_y_pos(make_note('b', 2), clef_sign::CLEF_BASS)) == 3);
  REQUIRE(near_int(calc_note_y_pos(make_note('d', 3), clef_sign::CLEF_BASS)) == 2);
  REQUIRE(near_int(calc_note_y_pos(make_note('f', 3), clef_sign::CLEF_BASS)) == 1);
  REQUIRE(near_int(calc_note_y_pos(make_note('a', 3), clef_sign::CLEF_BASS)) == 0);
  REQUIRE(near_int(calc_note_y_pos(make_note('c', 4), clef_sign::CLEF_BASS)) == -1);
  REQUIRE(near_int(calc_note_y_pos(make_note('e', 4), clef_sign::CLEF_BASS)) == -2);
}

