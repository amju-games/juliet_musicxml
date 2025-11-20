#pragma once

#include <string>
#include "expected.hpp" // TODO <expected> when available
#include "score.h"

namespace juliet_musicxml
{
using expected_score = tl::expected<score, std::string>;

class i_input_parser 
{
public:
  virtual ~i_input_parser() = default;

  // Parse a MusicXML file
  virtual expected_score parse_file(const std::string& filename) = 0;

  // Parse a string snippet of MusicXML
  virtual expected_score parse_string(const std::string& string) = 0;
};
}


