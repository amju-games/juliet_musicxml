#include <sstream>
#include "chord.h"
#include "i_renderer.h"

namespace juliet_musicxml
{
std::string chord::get_description() const
{
 std::stringstream ss; 
  ss << "CHORD:" << std::endl;
  for (const auto& e: m_events)
  {
    ss << "  " << e->get_description() << std::endl;
  }
  return ss.str();
}

void chord::render(i_renderer& r) const 
{
  r.render_chord(*this);

  // Just temporary  TODO TEMP TEST
  for (const auto& child : m_events) 
  {
    child->render(r); // Recursively render children
  }
}
}

