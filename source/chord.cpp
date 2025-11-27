#include <sstream>
#include "chord.h"

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
}

