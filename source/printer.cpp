#include <ostream>
#include <sstream>
#include "primitive_coords.h"
#include "printer.h"
#include "render_output.h"
#include "renderer.h"

namespace juliet_musicxml
{
namespace internal
{
std::ostream& operator<<(std::ostream& os, const coord& c)
{
  return os << "(" << c.first << ", " << c.second << ")";
}

template <typename ITER>
std::string comma_separated(ITER first, ITER last)
{
  std::stringstream ss;
  for (ITER it = first; it != (last -1); ++it)
  {
    ss << *it << ", ";
  }
  ss << *(last - 1);
  return ss.str();
}

std::string primitive_to_string(const description_output& d)
{
  // 1. Prepend '#' to cover the first line.
  std::string output = "# " + d;

  // 2. Iterate through the string and replace all occurrences of '\n' with '\n#'.
  size_t pos = 0;
  while ((pos = output.find('\n', pos)) != std::string::npos) 
  {
    // We found a newline. Replace it with '\n# '.
    output.replace(pos, 1, "\n# ");
        
    // Move the position past the inserted '\n#' (which is 2 characters long)
    // to continue the search for the next newline.
    pos += 2;
  }

  return output;
}

std::string primitive_to_string(const text_output& t)
{
  std::stringstream ss;
  ss << "text" << std::get<0>(t) << ", " << std::get<1>(t) << ", " << std::get<2>(t);
  return ss.str();
}

std::string primitive_to_string(const quad_output& q)
{
  std::stringstream ss;
  ss << "quad: " << comma_separated(q.begin(), q.end());
  return ss.str();
}

std::string primitive_to_string(const curve_output& c)
{
  std::stringstream ss;
  ss << "curve: " << comma_separated(c.begin(), c.end());
  return ss.str(); 
}

std::string primitive_to_string(const glyph_output& g)
{
  std::stringstream ss;
  ss << "glyph: " << std::get<0>(g) << ", " << std::get<1>(g);
  return ss.str();
}

std::string to_string(const render_output_primitive& primitive_variant)
{
  std::string ret;
  std::visit(
    [&](const auto& actual_primitive) 
    {
      ret = primitive_to_string(actual_primitive);
    }, primitive_variant);
  return ret;
}

std::string to_string(const render_output_event& e)
{
  std::string result;
  for (const render_output_primitive& child : e)
  {
    result += to_string(child); 
    result += "\n"; // TODO configurable char/string to delineate events/primitives
  }
  return result;
}

std::string to_string(const render_output& r)
{
  std::string result;
  for (const auto& e : r)
  {
    result += to_string(e); // + "\n"; // TODO Configurable
  }
  return result;
}
} // internal

std::string printer::to_string(const render_output& r) const
{
  return internal::to_string(r);
}
} // juliet_musicxml
 
