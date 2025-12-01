#include <iostream>
#include "catch.hpp"

// TODO!
//#include "../juliet_musicxml.hpp" // as this is an integration test

#include "interleaver.h"
#include "print_helper.h"
#include "text_renderer.h"
#include "time_normalizer.h"
#include "verticalizer.h"
#include "xml_parser.h"

bool parse_and_print(const std::string& filename);

TEST_CASE("Show events in two bar example", "time_normalizer")
{
  REQUIRE(parse_and_print("xml/two_bars.xml"));
}

/*
TEST_CASE("Show events in chopin example", "time_normalizer")
{
  REQUIRE(parse_and_print("xml/chopin.xml"));
}

TEST_CASE("Show events in two bar example", "time_normalizer")
{
  REQUIRE(parse_and_print("xml/two_bars.xml"));
}

TEST_CASE("Show events in Beethoven example", "time_normalizer")
{
  REQUIRE(parse_and_print("xml/beethoven.xml"));
}
*/

bool parse_and_print(const std::string& filename)
{
  using namespace juliet_musicxml;

  xml_parser parser;
  auto es = parser.parse_file(filename);
  score& s = es.value();

  time_normalizer tn; 
  tn.normalize_times(s);

  // TODO split up tasks
  event_vec verticals = interleaver::interleave_score_parts(s);
  std::cout << filename << ":\n";
  print_events(verticals);

  // TODO Horizontalizer

  text_renderer r(std::make_unique<final_text_render_output>());
  for (const auto& e : verticals)
  {
    e->render(r);
  }
  std::cout << r.output().to_string() << std::endl;
  
  return true;
}

