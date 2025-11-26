#include <iostream>
#include "catch.hpp"
#include "event.h"
#include "print_helper.h"
#include "verticalizer.h"

namespace juliet_musicxml
{
struct test_event : public event
{
  test_event(const std::string& description, const fraction& start_time) : 
    m_description(description) 
  {
    m_normalized_start_time = start_time;
  }

  std::string get_description() const override 
  {
    return m_description;
  }

  std::string m_description;
};

void check_event_is_vertical(event_vec& events, int index, int num_children)
{
  REQUIRE(dynamic_cast<vertical*>(events[index].get())); 
  vertical* v = dynamic_cast<vertical*>(events[index].get());
  REQUIRE(v->m_events.size() == num_children);
}
}

TEST_CASE("Group verticals", "verticalizer")
{
  using namespace juliet_musicxml;

  event_vec events;
  events.emplace_back(std::make_unique<test_event>("G",      fraction(0, 8)));
  events.emplace_back(std::make_unique<test_event>("A",      fraction(1, 8)));
  events.emplace_back(std::make_unique<test_event>("C (CH)", fraction(1, 8)));
  events.emplace_back(std::make_unique<test_event>("E (CH)", fraction(1, 8)));
  events.emplace_back(std::make_unique<test_event>("G (CH)", fraction(1, 8)));
  events.emplace_back(std::make_unique<test_event>("A",      fraction(2, 8)));
  events.emplace_back(std::make_unique<test_event>("C (CH)", fraction(2, 8)));
  events.emplace_back(std::make_unique<test_event>("E (CH)", fraction(2, 8)));
  events.emplace_back(std::make_unique<test_event>("B",      fraction(3, 8)));

  //std::cout << "Events before grouping verticals:" << std::endl;
  //print_events(events);
  
  verticalizer v;
  v.group_verticals(events);

  REQUIRE(events.size() == 4);
  check_event_is_vertical(events, 0, 1);
  check_event_is_vertical(events, 1, 4);
  check_event_is_vertical(events, 2, 3);
  check_event_is_vertical(events, 3, 1);
 
  //std::cout << "Events after grouping verticals:" << std::endl;
  //print_events(events);
}


