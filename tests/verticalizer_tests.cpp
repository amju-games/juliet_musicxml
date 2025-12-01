#include <iostream>
#include "catch.hpp"
#include "chord.h"
#include "event.h"
#include "make_event_helpers.h"
#include "print_helper.h"
#include "test_event.h"
#include "verticalizer.h"

namespace juliet_musicxml
{
// Check that the given event is a vertical, with the given number of children
//  and the given start time.
void check_event_is_vertical(const p_event& e, int num_children, int time)
{
  REQUIRE(dynamic_cast<vertical*>(e.get())); 
  vertical* v = dynamic_cast<vertical*>(e.get());
  REQUIRE(v->m_children.size() == num_children);
  REQUIRE(v->m_normalized_start_time == fraction(time, 1));
}

// Check that the given event is a chord, with the given number of notes
void check_event_is_chord(const p_event& e, int num_notes)
{
  REQUIRE(dynamic_cast<chord*>(e.get()));
  chord* c = dynamic_cast<chord*>(e.get());
  REQUIRE(c->m_children.size() == num_notes);
  // (Could add a check here that all children are 'note' objects)
}
}

// Getting Gemini to write some tests :)

TEST_CASE("V-01 Sequential Events Split", "[verticals]")
{
  using namespace juliet_musicxml;

  // Events: Note 1 @ Time 1, Note 2 @ Time 3 (Two verticals expected)
  event_vec events;
  events.emplace_back(make_note("c", 1, 1)); // Start Time 1
  events.emplace_back(make_note("d", 3, 1)); // Start Time 3

  verticalizer v;
  v.group_verticals(events);

  // 1. Check total number of verticals
  REQUIRE(events.size() == 2);

  // 2. Check the first vertical
  check_event_is_vertical(events[0], 1, 1);

  // 3. Check the second vertical
  check_event_is_vertical(events[1], 1, 3);
}

TEST_CASE("V-02 Simple Chord Grouping", "[verticals][chords]")
{
  using namespace juliet_musicxml;

  event_vec events;
  events.emplace_back(make_note("a", 2, 1));
  events.emplace_back(make_note("c", 2, 1)); // Same time, same voice

  verticalizer v;
  v.group_verticals(events);

  // 1. Check total number of verticals
  REQUIRE(events.size() == 1);

  // 2. Check the vertical: Time 2, 1 CHILD (which is the chord)
  check_event_is_vertical(events[0], 1, 2);

  // 3. Get that child and check it's a chord with 2 notes
  vertical* v_ptr = dynamic_cast<vertical*>(events[0].get());
  check_event_is_chord(v_ptr->m_children[0], 2);
}

TEST_CASE("V-03 Mixed Notes and Rests", "[verticals][chords]")
{
  using namespace juliet_musicxml;

  event_vec events;
  events.emplace_back(make_note("g", 5, 1));
  events.emplace_back(make_rest(5)); // Same time

  verticalizer v;
  v.group_verticals(events);

  // 1. Check total number of verticals
  REQUIRE(events.size() == 1);

  // 2. Check the vertical: Time 5, 2 CHILDREN (note and rest)
  check_event_is_vertical(events[0], 2, 5);
}

TEST_CASE("V-04 Complex Sequencing and Chords", "[verticals][chords]")
{
  using namespace juliet_musicxml;

  event_vec events;
  events.emplace_back(make_note("c", 1, 1)); // E1 @ Time 1 (Voice 1)
  events.emplace_back(make_note("g", 1, 2)); // E2 @ Time 1 (Voice 2)
  events.emplace_back(make_rest(3));     // E3 @ Time 3
  events.emplace_back(make_note("e", 5, 1)); // E4 @ Time 5 (Voice 1)
  events.emplace_back(make_note("g", 5, 1)); // E5 @ Time 5 (Voice 1)

  verticalizer v;
  v.group_verticals(events);

  // 1. Check total number of verticals
  REQUIRE(events.size() == 3);

  // 2. Check Vertical 1 (Time 1): 2 children (Note C, Note G - different voices)
  check_event_is_vertical(events[0], 2, 1);

  // 3. Check Vertical 2 (Time 3): 1 child (Rest)
  check_event_is_vertical(events[1], 1, 3);

  // 4. Check Vertical 3 (Time 5): 1 child (the E/G chord)
  check_event_is_vertical(events[2], 1, 5);

  // 5. Get the child from Vertical 3 and check it's a chord with 2 notes
  vertical* v_ptr = dynamic_cast<vertical*>(events[2].get());
  check_event_is_chord(v_ptr->m_children[0], 2);
}

TEST_CASE("V-05 Multi-Chord Vertical", "[verticals][chords]")
{
  using namespace juliet_musicxml;

  event_vec events;
  // Input events are unsorted
  events.emplace_back(make_note("g", 10, 2)); // Voice 2
  events.emplace_back(make_note("c", 10, 1)); // Voice 1
  events.emplace_back(make_note("e", 10, 1)); // Voice 1 (Chord with C)
  events.emplace_back(make_note("b", 10, 2)); // Voice 2 (Chord with G)

  verticalizer v;
  v.group_verticals(events);

  // 1. Check total number of verticals
  REQUIRE(events.size() == 1);

  // 2. Check the vertical: Time 10, 2 CHILDREN (Chord 1 and Chord 2)
  check_event_is_vertical(events[0], 2, 10);

  // 3. Get the vertical and its children
  vertical* v_ptr = dynamic_cast<vertical*>(events[0].get());
  auto& children = v_ptr->m_children;

  // 4. Check that BOTH children are chords.
  // We can't guarantee the order (Voice 1 chord or Voice 2 chord first).
  chord* c1 = dynamic_cast<chord*>(children[0].get());
  chord* c2 = dynamic_cast<chord*>(children[1].get());

  REQUIRE(c1); // Verify first child is a chord
  REQUIRE(c2); // Verify second child is a chord

  // 5. Check that both chords have 2 notes each.
  // We check the sizes in an order-agnostic way.
  bool sizes_are_correct = 
    (c1->m_children.size() == 2 && c2->m_children.size() == 2);
  REQUIRE(sizes_are_correct);
}

// Puny-human-written tests follow

TEST_CASE("Group verticals", "verticalizer")
{
  using namespace juliet_musicxml;

  event_vec events;
  events.emplace_back(std::make_unique<test_event>("G",      fraction(0, 1)));
  events.emplace_back(std::make_unique<test_event>("A",      fraction(1, 1)));
  events.emplace_back(std::make_unique<test_event>("C (CH)", fraction(1, 1)));
  events.emplace_back(std::make_unique<test_event>("E (CH)", fraction(1, 1)));
  events.emplace_back(std::make_unique<test_event>("G (CH)", fraction(1, 1)));
  events.emplace_back(std::make_unique<test_event>("A",      fraction(2, 1)));
  events.emplace_back(std::make_unique<test_event>("C (CH)", fraction(2, 1)));
  events.emplace_back(std::make_unique<test_event>("E (CH)", fraction(2, 1)));
  events.emplace_back(std::make_unique<test_event>("B",      fraction(3, 1)));

  verticalizer v;
  v.group_verticals(events);

  REQUIRE(events.size() == 4);
  check_event_is_vertical(events[0], 1, 0);
  check_event_is_vertical(events[1], 4, 1);
  check_event_is_vertical(events[2], 3, 2);
  check_event_is_vertical(events[3], 1, 3);
}

// Attributes with same start time as notes etc should get put into
//  separate verticals.
TEST_CASE("A-01 Attribute Isolation", "[attributes]")
{
  using namespace juliet_musicxml;

  // Events: Clef @ Time 1, Note @ Time 1 (Two verticals expected)
  event_vec events;
  
  // NOTE: We place the note first in the vector. 
  // The verticalizer must sort them and group them correctly.
  events.emplace_back(make_note("c", 1, 1));
  events.emplace_back(make_clef(1, "G")); 

  sort(events); // TODO Currently not done in verticalizer

  verticalizer v;
  v.group_verticals(events);

  // 1. Check total number of verticals
  REQUIRE(events.size() == 2);

  // 2. Check Vertical 1: The Attribute. 
  // Due to sorting logic, the attribute must come first.
  check_event_is_vertical(events[0], 1, 1);
  
  // Optional detailed check: Verify the type of the child (optional but good).
  vertical* v0_ptr = dynamic_cast<vertical*>(events[0].get());
  REQUIRE(dynamic_cast<clef_event*>(v0_ptr->m_children[0].get()));

  // 3. Check Vertical 2: The Rhythmic Event.
  check_event_is_vertical(events[1], 1, 1);
  
  // Optional detailed check: Verify the type of the child.
  vertical* v1_ptr = dynamic_cast<vertical*>(events[1].get());
  REQUIRE(dynamic_cast<note*>(v1_ptr->m_children[0].get()));
}

