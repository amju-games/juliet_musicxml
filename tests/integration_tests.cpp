#include <iostream>
#include "catch.hpp"
#include "interleaver.h"
#include "print_helper.h"
#include "printer.h"
#include "renderer.h"
#include "time_normalizer.h"
#include "verticalizer.h"
#include "xml_parser.h"

// These tests test the end-to-end behaviour but not the 
//  library file structure (single header)
//#include "../juliet_musicxml.hpp" 

bool parse_and_print(const std::string& filename);

// [.] disables a test

TEST_CASE("Simple example", "integration_test")
{
  REQUIRE(parse_and_print("xml/simple.xml"));
}

TEST_CASE("Chopin example", "integration_test")
{
  REQUIRE(parse_and_print("xml/chopin.xml"));
}

TEST_CASE("Two bar example", "[.]integration_test")
{
  REQUIRE(parse_and_print("xml/two_bars.xml"));
}

TEST_CASE("Beethoven example", "[.]integration_test")
{
  REQUIRE(parse_and_print("xml/beethoven.xml"));
}

bool parse_and_print(const std::string& filename)
{
  using namespace juliet_musicxml;

  xml_parser parser;
  auto es = parser.parse_file(filename);
  score& s = es.value();

  time_normalizer tn; 
  tn.normalize_times(s);

  event_vec verticals = interleaver::interleave_and_create_verticals(s);
  std::cout << filename << ":\n";

  // This should be a juliet_musicxml function, not here.
  renderer r;
  for (const auto& e : verticals)
  {
    e->render(r);
  }

  printer p;
  std::cout << p.to_string(r.output()) << std::endl;
  
  return true;
}

