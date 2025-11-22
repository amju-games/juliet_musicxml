#include <iostream>
#include "catch.hpp"
#include "xml_helper.h"
#include "xml_parser.h"

namespace juliet_musicxml
{
// Helper functions: the intention is to parse a focused snippet of xml.
// The xml snippet should be something that lives in a <measure>.

score parse_score(const std::string& xml_snippet)
{
  // Put a bit of boilerplate around the xml we want to test.
  std::string xml = "<score><part id=\"THE+PART\"><measure>" + xml_snippet + "</measure></part></score>"; 
  xml_parser p;
  auto result = p.parse_string(xml);
  if (!result)
  {
    std::cout << result.error() << std::endl 
      << "Parsing this rubbish: " << xml_snippet << std::endl;
  }
  REQUIRE(result);
  return std::move(result.value());
}

bar parse_bar(const std::string& xml_snippet)
{
  auto s = parse_score(xml_snippet);
  REQUIRE(!s.parts.empty());
  const auto it = s.parts.begin();
  REQUIRE(it->first == "THE+PART");
  auto& bars = it->second;
  REQUIRE(!bars.empty());
  return std::move(bars[0]);
}

// Parse a single XML event, e.g. a note
p_event parse_event(const std::string& xml_snippet)
{
  auto b = parse_bar(xml_snippet);
  REQUIRE(!b.events.empty());
  return std::move(b.events[0]);
}
}

