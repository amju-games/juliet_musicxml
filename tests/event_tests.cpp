#include "catch.hpp"
#include "event.h"

TEST_CASE("Pitch string", "pitch")
{
  juliet_musicxml::pitch p;
  p.m_step = 'g';
  p.m_alter = 1;
  p.m_octave = 3;
  REQUIRE(p.to_string() == "g#3");

  p.m_step = 'A';
  p.m_alter = -2;
  p.m_octave = 2;
  REQUIRE(p.to_string() == "Abb2");

  p.m_step = 'B';
  p.m_alter = 0;
  p.m_octave = 5;
  REQUIRE(p.to_string() == "B5");

  p.m_step = 'c';
  p.m_alter = 2;
  p.m_octave = 4;
  REQUIRE(p.to_string() == "c##4");

  p.m_step = 'D';
  p.m_alter = -1;
  p.m_octave = 6;
  REQUIRE(p.to_string() == "Db6");

  p.m_step = 'E';
  p.m_alter = 0;
  p.m_octave = 4;
  REQUIRE(p.to_string() == "E4");

  p.m_step = 'H';
  p.m_alter = 2;
  p.m_octave = 4;
  REQUIRE(p.to_string() == "X");

  p.m_step = 'x';
  p.m_alter = 0;
  p.m_octave = 7;
  REQUIRE(p.to_string() == "X");
}

TEST_CASE("Calc midi pitch - succeed", "pitch")
{
  // These pitches are in range and succeed

  juliet_musicxml::pitch p;

  p.m_step = 'g';
  p.m_alter = 1;
  p.m_octave = 3;
  auto r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 68);

  p.m_step = 'A';
  p.m_alter = -2;
  p.m_octave = 2;
  r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 55);

  p.m_step = 'B';
  p.m_alter = 0;
  p.m_octave = 5;
  r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 95);

  p.m_step = 'c';
  p.m_alter = 2;
  p.m_octave = 4;
  r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 74);

  p.m_step = 'D';
  p.m_alter = -1;
  p.m_octave = 6;
  r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 97);

  p.m_step = 'E';
  p.m_alter = 0;
  p.m_octave = 4;
  r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 76);
}

TEST_CASE("Edges of range", "pitch")
{
  using namespace juliet_musicxml;
  pitch p;

  p.m_step = 'c';
  p.m_alter = 0;
  p.m_octave = -2;
  auto r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 0);

  p.m_alter = -1;
  r = p.calc_midi_pitch();
  REQUIRE_FALSE(r);
  REQUIRE(r.error() == pitch_error::PITCH_TOO_LOW);

  p.m_step = 'g';
  p.m_alter = 0;
  p.m_octave = 8;
  r = p.calc_midi_pitch();
  REQUIRE(r);
  REQUIRE(r.value() == 127);

  p.m_alter = 1; 
  r = p.calc_midi_pitch();
  REQUIRE_FALSE(r);
  REQUIRE(r.error() == pitch_error::PITCH_TOO_HIGH);
}

TEST_CASE("Bad step", "pitch")
{
  // Bad step char -> fails

  using namespace juliet_musicxml;
  pitch p;

  p.m_step = 'H';
  p.m_alter = 2;
  p.m_octave = 4;
  auto r = p.calc_midi_pitch();
  REQUIRE_FALSE(r); // fails
  REQUIRE(r.error() == pitch_error::PITCH_BAD_STEP);

  p.m_step = 'x';
  p.m_alter = 0;
  p.m_octave = 7;
  r = p.calc_midi_pitch();
  REQUIRE_FALSE(r); // fails
  REQUIRE(r.error() == pitch_error::PITCH_BAD_STEP);
}

