#pragma once

#include "i_input_parser.h"

namespace juliet_musicxml
{
class xml_parser : public i_input_parser
{
public:
  expected_score parse_file(const std::string& filename) override;
  expected_score parse_string(const std::string& string) override;
};
}

