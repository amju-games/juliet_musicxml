#include <sstream>
#include "i_renderer.h"
#include "vertical.h"

namespace juliet_musicxml
{
vertical::vertical(event_vec::iterator first, event_vec::iterator last)
{
  m_children.insert(m_children.begin(), std::make_move_iterator(first), std::make_move_iterator(last));
  // Copy the start time from first child to vertical. All children should have the same start
  //  time, that's why they are in the same vertical :)
  if (first != last) // why would vertical be empty tho?
  {
    m_normalized_start_time = m_children.front()->m_normalized_start_time;
  }
}

std::string vertical::get_description() const
{
  std::stringstream ss;
  ss << "VERTICAL: (x: " << m_info.m_x_offset << ", start time: " << m_normalized_start_time << ")" << std::endl;
  for (const auto& e: m_children)
  {
    ss << "  " << e->get_description() << std::endl;
  }
  return ss.str();
}

void vertical::render(i_renderer& r) const 
{
  r.render_vertical(*this);
 
  composite_event::render(r); 
}
}

