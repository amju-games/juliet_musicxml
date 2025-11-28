#include <iostream>
#include "catch.hpp"
#include "xml_helper.h"
#include "xml_parser.h"

TEST_CASE("Parse simple xml file example", "xml_parser")
{
  // Test parsing of simplest full example, having one part, one bar,
  //  one semibreve note. Treble clef, 4/4 time sig, C major key sig.

  using namespace juliet_musicxml;

  xml_parser parser;
  auto result = parser.parse_file("xml/simple.xml");
  REQUIRE(result);
  const auto& score = result.value();
  REQUIRE(score.parts.size() == 1); // expect one part
  const auto it = score.parts.begin();
  const auto& bars = it->second;
  REQUIRE(!bars.empty());
  const auto& bar = bars[0];
  REQUIRE(bar->events.size() == 4); // number of events in the bar in the example XML
  // First event should be Music XML divisions
  REQUIRE(dynamic_cast<divisions*>(bar->events[0].get())); 
  const auto* divs = dynamic_cast<divisions*>(bar->events[0].get());
  REQUIRE(divs->m_num_divisions == 48); // depends on contents of file, not a good test
  // Second event: clef attributes for the bar
  REQUIRE(dynamic_cast<clef_event*>(bar->events[1].get())); 
  const auto* clefs = dynamic_cast<clef_event*>(bar->events[1].get());
  REQUIRE(clefs->m_clef_map.size() == 1);
  REQUIRE(clefs->m_clef_map.at(1).m_sign == "G");
  const auto* time_sig = dynamic_cast<time_sig_event*>(bar->events[2].get());
  REQUIRE(time_sig->m_fraction.num == 4);
  REQUIRE(time_sig->m_fraction.denom == 4);
  // No key sig because nothing renderable.
  // This would be different if naturalizing a previous time sig.
  const auto* n = dynamic_cast<note*>(bar->events[3].get());

  // Check part 
  REQUIRE(bar->part_id == "P1"); // part name should match what's in the example XML

  // TODO Other attribute event types
}

TEST_CASE("Parse two-bar xml file example", "xml_parser")
{
  // Test first-pass parsing of two-bar G major example.

  using namespace juliet_musicxml;

  xml_parser p;
  auto result = p.parse_file("xml/two_bars.xml");
  REQUIRE(result);
  const auto& score = result.value();
  REQUIRE(score.parts.size() == 1); // one part
  const auto& bars = score.parts.begin()->second;
  REQUIRE(bars.size() == 2); // expect two bars
  const auto& bar = bars[0];
  const auto& event_1 = bar->events[0];
  const auto* divs = dynamic_cast<divisions*>(event_1.get());
  REQUIRE(divs->m_num_divisions == 1);
  const auto& event_2 = bar->events[1];
  const auto* clefs = dynamic_cast<clef_event*>(event_2.get());
  REQUIRE(clefs);
  REQUIRE(clefs->m_clef_map.size() == 1); // one clef in XML file

  // TODO Key sig
//  REQUIRE(attrs->m_key_sig == key_sig::KEYSIG_1_SHARP);
}

TEST_CASE("Parse attribs: 4 flats key sig", "xml_parser internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event("<attributes><key><fifths>-4</fifths></key></attributes>");
  const auto* key_sig = dynamic_cast<key_sig_event*>(event.get());
  REQUIRE(key_sig);
  REQUIRE(key_sig->m_key_sig == key_sig::KEYSIG_4_FLAT);
}

TEST_CASE("Parse attribs: num staves", "xml_parser internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event("<attributes><staves>2</staves></attributes>");
  const auto* staves = dynamic_cast<stave_event*>(event.get());
  REQUIRE(staves->m_num_staves == 2);
}

TEST_CASE("Parse attribs: divisions", "xml_parser internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event("<attributes><divisions>7</divisions></attributes>");
  const auto* divs = dynamic_cast<divisions*>(event.get());
  REQUIRE(divs->m_num_divisions == 7);
}

TEST_CASE("Parse attribs: clefs", "xml_parser internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event(R"(
<attributes><clef number="1"><sign>F</sign><line>4</line></clef><clef number="2"><sign>G</sign><line>2</line></clef></attributes>
  )");

  const auto* clefs = dynamic_cast<clef_event*>(event.get());
  REQUIRE(clefs->m_clef_map.size() == 2);
  REQUIRE(clefs->m_clef_map.at(1).m_sign == "F");
  REQUIRE(clefs->m_clef_map.at(1).m_line == 4);
  REQUIRE(clefs->m_clef_map.at(2).m_sign == "G");
  REQUIRE(clefs->m_clef_map.at(2).m_line == 2);
}

TEST_CASE("Parse note: staff", "xml_parser_internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event("<note><staff>42</staff></note>");
  const auto* n = dynamic_cast<note*>(event.get());
  REQUIRE(n->m_staff == 42);
}

TEST_CASE("Parse note: stem dir", "xml_parser_internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event("<note><stem>up</stem></note>");
  const auto* n = dynamic_cast<note*>(event.get());
  REQUIRE(n->m_stem.m_direction == stem::direction::STEM_UP);
}

TEST_CASE("Parse note: voice", "xml_parser_internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event("<note><voice>23</voice></note>");
  const auto* n = dynamic_cast<note*>(event.get());
  REQUIRE(n->m_voice == 23);
}

TEST_CASE("Parse note: alter", "xml_parser_internals")
{
  using namespace juliet_musicxml;
  const auto event = parse_event("<note><alter>-1</alter></note>");
  const auto* n = dynamic_cast<note*>(event.get());
  REQUIRE(n->m_pitch.m_alter == -1);
}

TEST_CASE("Parse note, rest, rest, note", "xml_parser")
{
  using namespace juliet_musicxml;

  const auto bar = parse_bar(R"(
<note><step>C</step><duration>5</duration></note>
<note><staff>3</staff><voice>2</voice><rest measure="yes"></rest><duration>5</duration></note>
<note><duration>7</duration><rest/></note>
<note><step>D</step><duration>6</duration></note>
)");
  
  REQUIRE(dynamic_cast<note*>(bar->events[0].get())); 
  REQUIRE(dynamic_cast<rest*>(bar->events[1].get())); 
  REQUIRE(dynamic_cast<rest*>(bar->events[2].get()));
  REQUIRE(dynamic_cast<note*>(bar->events[3].get())); // make sure flag is reset

  note* n = dynamic_cast<note*>(bar->events[0].get());
  // Expect note and rest staves to default to 1
  REQUIRE(n->m_staff == 1); 

  rest* r = dynamic_cast<rest*>(bar->events[1].get());
  REQUIRE(r->m_duration == 5);
  REQUIRE(r->m_staff == 3);
  REQUIRE(r->m_voice == 2);
  REQUIRE(r->m_is_whole_bar == true);

  r = dynamic_cast<rest*>(bar->events[2].get());
  REQUIRE(r->m_duration == 7);
  REQUIRE(r->m_staff == 1);
  REQUIRE(r->m_voice == 1);
  REQUIRE(r->m_is_whole_bar == false);
}

