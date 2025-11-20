#pragma once

#include "expected.hpp" // TODO <expected> when available
#include "bar.h"
#include "fraction.h"
#include "score.h"
#include "vertical.h"

namespace juliet_musicxml
{
// TODO Should be using an enum for errors, not a string?
using expected_vertical_vec = tl::expected<vertical_vec, std::string>;

class verticalizer
{
public:

  // Urgh, the problem here is we want to treat the events polymorphically --
  //  or, not even that, just use the fact that all the types have a common
  //  base class. std::variant is completely unhelpful. 
  // It doesn't make a whole lot of sense to have a variant and then switch
  //  to unique ptrs. Argh we need an intern to change everything over to
  //  ptrs.... can an AI do it?

  expected_vertical_vec group_verticals(event_vec);
};
}

