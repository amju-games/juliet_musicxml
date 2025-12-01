#pragma once

#include "event.h"

namespace juliet_musicxml
{
// Info to render a vertical - not page info, stuff like relative width of
//  this vertical.
struct vertical_render_info
{
};

// Vertical: info for rendering plus vec of all renderable events that
//  are vertically aligned
struct vertical : public composite_event
{
  // Construct vertical by moving a range of events
  vertical(event_vec::iterator first, event_vec::iterator last);

  std::string get_description() const override;
  void render(i_renderer&) const override;

  vertical_render_info m_render_info;
};
}

