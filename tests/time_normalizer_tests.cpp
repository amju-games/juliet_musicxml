#include <iostream>
#include "catch.hpp"
#include "tinyxml2.h"
#include "time_normalizer.h"
#include "xml_helper.h"
#include "xml_parser.h"

void print_events(const juliet_musicxml::event_vec& events)
{
  using namespace juliet_musicxml;
  for (const auto& evu : events) 
  {
    if (std::holds_alternative<note>(evu))
    {
      std::cout << std::get<note>(evu).m_description << std::endl;
    }
  }
}

bool parse_and_print(const std::string& filename)
{
  using namespace juliet_musicxml;
 
  xml_parser parser;
  const auto score = parser.parse_file(filename);
  REQUIRE(score);

  time_normalizer v;
  auto events = v.get_timed_events(score.value());
  print_events(events);

  return true;
}

TEST_CASE("Show events in chopin example", "time_normalizer")
{
  REQUIRE(parse_and_print("xml/chopin.xml"));
}

/*
TEST_CASE("Show events in two bar example", "time_normalizer")
{
  REQUIRE(parse_and_print("xml/two_bars.xml"));
}

TEST_CASE("Show events in Beethoven example", "time_normalizer")
{
  REQUIRE(parse_and_print("xml/beethoven.xml"));
}
*/

TEST_CASE("Get timed events in one bar, check times", "time_normalizer")
{
  using namespace juliet_musicxml;

  bar b;
  {
    note n;
    n.m_pitch.m_step = 'C';
    n.m_pitch.m_octave = 5;
    n.m_duration = 4;
    b.events.emplace_back(n);
  }

  backup bk;
  bk.m_duration = 2;
  b.events.emplace_back(bk);

  {
    note n;
    n.m_pitch.m_step = 'E';
    n.m_pitch.m_octave = 5;
    n.m_duration = 8;
    b.events.emplace_back(n);
  }

  time_normalizer tn;
  int ticks = 10;
  int divs = 1;
  fraction norm_time(ticks, divs);
  auto result = tn.get_timed_events(b, ticks, divs, norm_time);
  REQUIRE(ticks == 20); // Because we started at 10
  REQUIRE(norm_time == fraction(20, 1)); // as divisions not changed
  const auto& note1 = std::get<note>(result.front());
  REQUIRE(note1.m_normalized_start_time == fraction(10, 1));
  REQUIRE(note1.m_normalized_duration == fraction(4, 1));
  const auto& note2 = std::get<note>(result.back()); // skip over backup event if it's in there 
  REQUIRE(note2.m_normalized_start_time == fraction(12, 1));
  REQUIRE(note2.m_normalized_duration == fraction(8, 1));
}

// Convenience function
juliet_musicxml::note make_note(char step, int octave, int alter, int duration_ticks, std::string_view type)
{
  juliet_musicxml::note n;
  n.m_pitch.m_step = step;
  n.m_pitch.m_octave = octave;
  n.m_pitch.m_alter = alter;
  n.m_type = type;
  n.m_duration = duration_ticks;
  return n;
}

juliet_musicxml::divisions make_divisions(int num_divisions)
{
  juliet_musicxml::divisions d;
  d.m_num_divisions = num_divisions;
  return d;
}

TEST_CASE("Normalize start times in timed_events", "time_normalizer")
{
  using namespace juliet_musicxml;
  // Durations and accumulated start times depend on note durations
  //  and the prevailing number of divisions per crotchet.

  // This has to be resolved during the second pass, when we calc
  //  start times based on current time, <backup> and <forward>
  //  events.. right?
  bar rb;
  rb.events = 
  {
    make_divisions(8),
    make_note('C', 3, 0,  8, "quarter"),
    make_note('D', 3, 0, 16, "minim"), 

    make_divisions(5), // changing divisions!
    make_note('E', 3, 0,  5, "quarter"),
    make_note('F', 3, 0, 20, "whole"),
  };
 
  time_normalizer tn;
  int ticks = 0;
  int divs = 1;
  fraction norm_time;
  auto events = tn.get_timed_events(rb, ticks, divs, norm_time);
/*
  print_events(events);
*/

  REQUIRE(std::get<divisions>(events[0]).m_normalized_start_time == fraction(0, 1)); // divs 
  REQUIRE(std::get<note>(events[1]).m_normalized_start_time == fraction(0, 1)); // 'C'
  REQUIRE(std::get<note>(events[2]).m_normalized_start_time == fraction(1, 1)); // 'D'
  REQUIRE(std::get<divisions>(events[3]).m_normalized_start_time == fraction(3, 1)); // divs
  REQUIRE(std::get<note>(events[4]).m_normalized_start_time == fraction(3, 1)); // 'E'
  REQUIRE(std::get<note>(events[5]).m_normalized_start_time == fraction(4, 1)); // 'F'a

  REQUIRE(norm_time == fraction(8, 1));
  REQUIRE(divs == 5);
}

