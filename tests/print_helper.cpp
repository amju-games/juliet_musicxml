#include <iostream>
#include "interleaver.h"
#include "print_helper.h"
#include "time_normalizer.h"
#include "verticalizer.h"
#include "xml_parser.h"

using namespace juliet_musicxml;

void print_events(const event_vec& events)
{
  for (const auto& e : events) 
  {
    std::cout << e->get_description() << std::endl;
  }
}

void print_bars(const bar_vec& bars)
{
  for (const auto& b : bars)
  {
    std::cout << "Bar: " << b->bar_number << std::endl;
    print_events(b->events);
  }
}

void print_score(const score& s)
{
  for (const auto& [part, bars] : s.m_parts)
  {   
    std::cout << "Part: " << part << std::endl;
    print_bars(bars);
  }
}

