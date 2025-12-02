#include "bar.h"

namespace juliet_musicxml
{
void bar::set_part_index(int part_index)
{
  m_part_index = part_index;
  for (auto&e : m_events)
  {
    e->m_part_index = part_index;
  }
}
}

