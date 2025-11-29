#include <iostream>
#include "catch.hpp"
#include "print_helper.h"
#include "tinyxml2.h"
#include "time_normalizer.h"
#include "xml_helper.h"
#include "xml_parser.h"

using namespace juliet_musicxml;

TEST_CASE("Get timed events in one bar, check times", "time_normalizer")
{
  bar b;
  {
    note n;
    n.m_pitch.m_step = 'C';
    n.m_pitch.m_octave = 5;
    n.m_duration = 4;
    b.events.emplace_back(std::make_unique<note>(n));
  }

  backup bk;
  bk.m_duration = 2;
  b.events.emplace_back(std::make_unique<backup>(bk));

  {
    note n;
    n.m_pitch.m_step = 'E';
    n.m_pitch.m_octave = 5;
    n.m_duration = 8;
    b.events.emplace_back(std::make_unique<note>(n));
  }

  time_normalizer tn;
  int ticks = 10;
  int divs = 1;
  fraction norm_time(ticks, divs);
  tn.normalize_times(b, ticks, divs, norm_time);
  REQUIRE(ticks == 20); // Because we started at 10
  REQUIRE(norm_time == fraction(20, 1)); // as divisions not changed
  const auto& note1 = *b.events.front();
  REQUIRE(note1.m_normalized_start_time == fraction(10, 1));
  REQUIRE(note1.m_normalized_duration == fraction(4, 1));
  const auto& note2 = *b.events.back(); // skip over backup event if it's in there 
  REQUIRE(note2.m_normalized_start_time == fraction(12, 1));
  REQUIRE(note2.m_normalized_duration == fraction(8, 1));
}

// Convenience functions
auto make_note(
  char step, int octave, int alter, int duration_ticks, std::string_view type, bool is_chord = false)
{
  note n;
  n.m_pitch.m_step = step;
  n.m_pitch.m_octave = octave;
  n.m_pitch.m_alter = alter;
  n.m_type = type;
  n.m_duration = duration_ticks;
  n.m_is_chord = is_chord;
  return std::make_unique<note>(n);
}

auto make_rest(int duration_ticks, std::string_view type)
{
  rest r;
  r.m_duration = duration_ticks;
  return std::make_unique<rest>(r);
}

auto make_divisions(int num_divisions)
{
  divisions d;
  d.m_num_divisions = num_divisions;
  return std::make_unique<divisions>(d);
}

auto make_backup(int ticks)
{
  backup b;
  b.m_duration = ticks;
  return std::make_unique<backup>(b);
}

auto make_forward(int ticks)
{
  forward f;
  f.m_duration = ticks;
  return std::make_unique<forward>(f);
}

TEST_CASE("Normalize start times in events", "time_normalizer")
{
  // Durations and accumulated start times depend on note durations
  //  and the prevailing number of divisions per crotchet.
  // This has to be resolved during the second pass, when we calc
  //  start times based on current time, <backup> and <forward>
  //  events.

  // Aghh can't use initializer list with vec of unique ptrs
  bar b;
  b.events.emplace_back(make_divisions(8));
  b.events.emplace_back(make_note('C', 3, 0,  8, "quarter"));
  b.events.emplace_back(make_note('D', 3, 0, 16, "minim"));
  b.events.emplace_back(make_divisions(5)); // changing divisions!
  b.events.emplace_back(make_note('E', 3, 0,  5, "quarter"));
  b.events.emplace_back(make_note('F', 3, 0, 20, "whole"));
 
  time_normalizer tn;
  int ticks = 0;
  int divs = 1;
  fraction norm_time;
  tn.normalize_times(b, ticks, divs, norm_time);

  //print_events(b.events); // TODO TEMP TEST

  REQUIRE(b.events[0]->m_normalized_start_time == fraction(0, 1)); // divs 
  REQUIRE(b.events[1]->m_normalized_start_time == fraction(0, 1)); // 'C'
  REQUIRE(b.events[2]->m_normalized_start_time == fraction(1, 1)); // 'D'
  REQUIRE(b.events[3]->m_normalized_start_time == fraction(3, 1)); // divs
  REQUIRE(b.events[4]->m_normalized_start_time == fraction(3, 1)); // 'E'
  REQUIRE(b.events[5]->m_normalized_start_time == fraction(4, 1)); // 'F'a

  REQUIRE(norm_time == fraction(8, 1));
  REQUIRE(divs == 5);
}

TEST_CASE("Rest start times", "time_normalizer")
{
  bar b;
  b.events.emplace_back(make_note('a', 3, 0,  8, "quarter")); 
  b.events.emplace_back(make_rest(4, "eighth"));
  b.events.emplace_back(make_backup(7));
  b.events.emplace_back(make_note('b', 3, 0,  16, "half")); 
  b.events.emplace_back(make_rest(2, "16th"));
  b.events.emplace_back(make_note('c', 3, 0,  4, "eighth")); 
  b.events.emplace_back(make_forward(3));
  b.events.emplace_back(make_rest(8, "quarter"));
 
  time_normalizer tn;
  int ticks = 0;
  int divs = 8;
  fraction norm_time;
  tn.normalize_times(b, ticks, divs, norm_time);

  REQUIRE(b.events[0]->m_normalized_start_time == fraction(0, 8)); 
  REQUIRE(b.events[1]->m_normalized_start_time == fraction(8, 8)); 
  REQUIRE(b.events[3]->m_normalized_start_time == fraction(5, 8)); 
  REQUIRE(b.events[4]->m_normalized_start_time == fraction(21, 8)); 
  REQUIRE(b.events[5]->m_normalized_start_time == fraction(23, 8)); 
  REQUIRE(b.events[7]->m_normalized_start_time == fraction(30, 8)); 
}

TEST_CASE("Chords - note start times ", "time_normalizer")
{
  // Test that all notes in a chord have the same normalized start time.

  bar b;
  b.events.emplace_back(make_note('B', 3, 0,  8, "quarter")); // single note
  b.events.emplace_back(make_note('C', 3, 0,  4, "quarter"));  // 3-note chord
  b.events.emplace_back(make_note('E', 3, 0,  4, "quarter", true));
  b.events.emplace_back(make_note('G', 3, 0,  4, "quarter", true));
  b.events.emplace_back(make_backup(7));
  b.events.emplace_back(make_note('D', 3, 0,  6, "quarter"));  // 3-note chord
  b.events.emplace_back(make_note('F', 3, 1,  6, "quarter", true));
  b.events.emplace_back(make_note('A', 3, 0,  6, "quarter", true));
  b.events.emplace_back(make_forward(20));
  b.events.emplace_back(make_note('E', 3, 0,  5, "quarter"));  // 3-note chord
  b.events.emplace_back(make_note('G', 3, 1,  5, "quarter", true));
  b.events.emplace_back(make_note('B', 4, 0,  5, "quarter", true));
  
  time_normalizer tn;
  int ticks = 0;
  int divs = 3;
  fraction norm_time;
  tn.normalize_times(b, ticks, divs, norm_time);

  REQUIRE(b.events[0]->m_normalized_start_time == fraction(0, 3)); // B
  REQUIRE(b.events[1]->m_normalized_start_time == fraction(8, 3)); // C
  REQUIRE(b.events[2]->m_normalized_start_time == fraction(8, 3)); // E - same start time as C
  REQUIRE(b.events[3]->m_normalized_start_time == fraction(8, 3)); // G - ditto
  REQUIRE(b.events[5]->m_normalized_start_time == fraction(5, 3)); // D
  REQUIRE(b.events[6]->m_normalized_start_time == fraction(5, 3)); // F#
  REQUIRE(b.events[7]->m_normalized_start_time == fraction(5, 3)); // A
  REQUIRE(b.events[9]->m_normalized_start_time == fraction(31, 3)); // E 
  REQUIRE(b.events[10]->m_normalized_start_time == fraction(31, 3)); // G#
  REQUIRE(b.events[11]->m_normalized_start_time == fraction(31, 3)); // B
}

