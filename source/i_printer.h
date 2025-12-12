#pragma once

#include "render_output.h" // hmm can't fwd decl, it's a vector

namespace juliet_musicxml
{
// Interface for printer: 
// Converts render_output to string.
// The implementation of subclasses depends on how you want
//  the string output to look. 
class i_printer
{
public:
  virtual ~i_printer() = default;

  virtual std::string to_string(const render_output&) const = 0;
};
}

