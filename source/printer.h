#pragma once

#include "i_printer.h"

namespace juliet_musicxml
{
// Concrete printer class that converts render_output to string.
class printer : public i_printer
{
public:
  std::string to_string(const render_output&) const override;
};
}

