#include "catch.hpp"
#include "interleaver.h"
#include "make_event_helpers.h"
#include "score.h"
#include "verticalizer.h"

namespace juliet_musicxml
{
// Check that the given event is a vertical, with the given number of children
//  and the given start time.
void check_event_is_vertical(const p_event& e, int num_children, int time);

std::unique_ptr<bar> make_bar(event_vec&& events, int bar_length = 4)
{
  bar b;
  b.events = std::move(events);
  return std::make_unique<bar>(std::move(b));
}
}

TEST_CASE("P-01 Basic Two-Part Alignment", "[interleaving]")
{
  using namespace juliet_musicxml;

  score s;

  // --- Part 1 Setup ---
  event_vec p1_events;
  p1_events.emplace_back(make_note("c", 1, 1)); 
  
  bar_vec p1_bars; // Create the bar vector explicitly
  p1_bars.emplace_back(make_bar(std::move(p1_events))); // Add the bar
  
  s.add_new_part("P1", p1_bars);

  // --- Part 2 Setup ---
  event_vec p2_events;
  p2_events.emplace_back(make_note("g", 1, 1));

  bar_vec p2_bars;
  p2_bars.emplace_back(make_bar(std::move(p2_events)));
  
  s.add_new_part("P2", p2_bars);

  event_vec verticals = interleaver::interleave_score_parts(s);

  REQUIRE(verticals.size() == 1);
  check_event_is_vertical(verticals[0], 2, 1); // expect two note children, not a chord; time 1
}

TEST_CASE("P-02 Sequential Bar Interleaving", "[interleaving]")
{
  using namespace juliet_musicxml;

  score s;

  // --- Part 1 Setup ---
  event_vec p1_bar1_events;
  p1_bar1_events.emplace_back(make_note("a", 1, 1)); // a, time 1, voice 1
  
  event_vec p1_bar2_events;
  p1_bar2_events.emplace_back(make_note("e", 5, 1)); // e, time 5, voice 1

  bar_vec p1_bars; // Create the bar vector explicitly
  p1_bars.emplace_back(make_bar(std::move(p1_bar1_events)));
  p1_bars.emplace_back(make_bar(std::move(p1_bar2_events)));
  
  s.add_new_part("P1", p1_bars);

  // --- Part 2 Setup ---
  event_vec p2_bar1_events;
  p2_bar1_events.emplace_back(make_note("c", 3, 1)); // c, time 3, voice 1
  
  bar_vec p2_bars;
  p2_bars.emplace_back(make_bar(std::move(p2_bar1_events)));
  
  s.add_new_part("P2", p2_bars);

  event_vec verticals = interleaver::interleave_score_parts(s);

  REQUIRE(verticals.size() == 3);
  check_event_is_vertical(verticals[0], 1, 1); // Expect 1 note, time 1, in 1st part
  check_event_is_vertical(verticals[1], 1, 3); // Expect 1 note, time 3, in 2nd part
  check_event_is_vertical(verticals[2], 1, 5); // Expect 1 note, time 5, in 1st part
}

TEST_CASE("P-03 Misaligned Bar Starts", "[interleaving]")
{
  using namespace juliet_musicxml;

  score s;

  // --- Part 1 Setup ---
  event_vec p1_bar1_events;
  p1_bar1_events.emplace_back(make_note("a", 3, 1));
  
  event_vec p1_bar2_events;
  p1_bar2_events.emplace_back(make_note("b", 4, 1));

  bar_vec p1_bars;
  p1_bars.emplace_back(make_bar(std::move(p1_bar1_events), 4));
  p1_bars.emplace_back(make_bar(std::move(p1_bar2_events), 4));
  
  s.add_new_part("P1", p1_bars);

  // --- Part 2 Setup ---
  event_vec p2_bar1_events;
  p2_bar1_events.emplace_back(make_note("c", 5, 1));
  
  bar_vec p2_bars;
  p2_bars.emplace_back(make_bar(std::move(p2_bar1_events), 6));
  
  s.add_new_part("P2", p2_bars);

  event_vec verticals = interleaver::interleave_score_parts(s);

  REQUIRE(verticals.size() == 3);
  check_event_is_vertical(verticals[0], 1, 3); // a, time 3
  check_event_is_vertical(verticals[1], 1, 4); // b, time 4
  check_event_is_vertical(verticals[2], 1, 5); // b, time 5
}

TEST_CASE("P-04 Vertical Content Isolation", "[interleaving][chords]")
{
  using namespace juliet_musicxml;

  score s;
  
  // --- Part 1 Setup ---
  event_vec p1_events;
  p1_events.emplace_back(make_note("c", 1, 1));
  
  bar_vec p1_bars;
  p1_bars.emplace_back(make_bar(std::move(p1_events)));
  
  s.add_new_part("P1", p1_bars);

  // --- Part 2 Setup ---
  event_vec p2_events;
  p2_events.emplace_back(make_note("g", 1, 1));
  
  bar_vec p2_bars;
  p2_bars.emplace_back(make_bar(std::move(p2_events)));
  
  s.add_new_part("P2", p2_bars);

  event_vec verticals = interleaver::interleave_score_parts(s);

  REQUIRE(verticals.size() == 1);
  check_event_is_vertical(verticals[0], 2, 1); // 2 note children; not a chord.
  
  vertical* v0_ptr = dynamic_cast<vertical*>(verticals[0].get());
  REQUIRE(dynamic_cast<note*>(v0_ptr->m_children[0].get()));
  REQUIRE(dynamic_cast<note*>(v0_ptr->m_children[1].get()));
}

