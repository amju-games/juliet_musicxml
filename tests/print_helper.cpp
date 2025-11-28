#include <iostream>
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
  for (const bar& b : bars)
  {
    std::cout << "Bar: " << b.bar_number << std::endl;
    print_events(b.events);
  }
}

void print_score(const score& s)
{
  for (const auto& [part, bars] : s.parts)
  {   
    std::cout << "Part: " << part << std::endl;
    print_bars(bars);
  }
}

bool parse_and_print(const std::string& filename)
{
  xml_parser parser;
  auto es = parser.parse_file(filename);
  score& s = es.value();

  // TODO Description should be made on the fly in each event type,
  //  not during time normalization
  std::cout << "BEFORE NORMALIZING" << std::endl;
  print_score(s);

  time_normalizer tn;
  tn.normalize_times(s);

  std::cout << "AFTER NORMALIZING" << std::endl;
  print_score(s);

  verticalizer v;
  v.group_verticals(s);
  
  std::cout << "AFTER GROUPING VERTICALS" << std::endl;
  print_score(s);

  return true;
}

