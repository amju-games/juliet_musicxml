#include <sstream>
#include "vertical.h"

namespace juliet_musicxml
{
std::string vertical::get_description() const
{
  std::stringstream ss;
  ss << "VERTICAL:" << std::endl;
  for (const auto& e: m_events)
  {
    ss << "  " << e->get_description() << std::endl;
  }
  return ss.str();
}
}

