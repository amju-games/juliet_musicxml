#pragma once

#include "event.h"

namespace juliet_musicxml
{
// Info to render a vertical - not page info, stuff like relative width of
//  this vertical.
struct render_info
{
};

// Vertical: info for rendering plus vec of all renderable events that
//  are vertically aligned
struct vertical
{
  render_info m_render_info;
  event_vec m_events;
};

using vertical_vec = std::vector<vertical>;
}

