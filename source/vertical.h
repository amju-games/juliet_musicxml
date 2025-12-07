#pragma once

#include "event.h"

namespace juliet_musicxml
{
// Info to render a vertical - not page info, stuff like relative width of
//  this vertical.
struct vertical_render_info
{
  // Units of distance: 1.0 == the distance between two stave lines.

  float m_x_offset = 0; // Added to x-coord of every child of the vertical
  float m_width = 0; // max/accumulated width of descendants.
  float m_x_spacing = 0; // gap between this vertical and the next.
    // This is proportional to difference in start time, i.e. duration, 
    //  so verticals take up horizontal space according to their length.
    // But we might dial down this effect as we might need to save space.
    // We will have an overall min and max for the spacing, and use less
    //  of that range as we need to compress the score horizontally.
};

// Vertical: info for rendering plus vec of all renderable events that
//  are vertically aligned
struct vertical : public composite_event
{
  // Construct vertical by moving a range of events
  vertical(event_vec::iterator first, event_vec::iterator last);

  std::string get_description() const override;
  void render(i_renderer&) const override;

  vertical_render_info m_info;
};
}

