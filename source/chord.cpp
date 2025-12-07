#include <sstream>
#include "chord.h"
#include "i_renderer.h"

namespace juliet_musicxml
{
std::string chord::get_description() const
{
 std::stringstream ss; 
  ss << "CHORD:" << std::endl;
  for (const auto& e: m_children)
  {
    ss << "  " << e->get_description() << std::endl;
  }
  return ss.str();
}

void chord::render(i_renderer& r) const 
{
  r.render_chord(*this);
}
}

