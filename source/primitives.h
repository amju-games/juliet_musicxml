#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace juliet_musicxml
{
// Render primitives

// Coordinate system
// The unit of distance is the distance between two adjacent stave lines.
// (0, 0) is top left of page. Max x and y values are set in page_info.

// Coordinates and scale basic types
using x_coord = float; 
using y_coord = float;
using scale = float;

// 2D coord: position of anchor point for a glyph, or a corner of a quad,
//  or a control point for a line. 
// For text, it could specify the left edge of the text.
using coord = std::pair<x_coord, y_coord>;

// Glyphs are the individual elements of a music font.
// The string (first element) is a human-readable name for the glyph, 
//  e.g. "filled-note-head", which is later mapped to a character code.
// The coord (2nd element) defines the position of the anchor point 
//  for the given glyph.
using glyph_output = std::tuple<std::string, coord>;
  
// Perhaps we will also use this, to scale glyph size:
// using glyph_output_with_scale = std::tuple<std::string, coord, scale>;

// A curve (catmull-rom spline) defined by 4 or more coords:
using curve_output = std::vector<coord>;

// A filled quadrilateral, defined by 4 corner coords:
// Winding order is CCW.
using quad_output = std::array<coord, 4>;

// Text (TODO in future: refine with font and style info):
using text_output = std::tuple<std::string, coord, scale>;

// Event description: this is a comment used in testing to distinguish
//  groups of primitives in the output. Strip this from production output.
using description_output = std::string;

// One primitive renderable element: one of the above output types.
using render_output_primitive = std::variant<
  glyph_output,
  curve_output,
  quad_output,
  text_output,
  description_output>;

// One 'music event' (e.g. a note, a clef, etc) is a collection of one
//  or more of the above primitives. E.g. a note might consist of two
//  primitives: a note-head glyph, and a stem, (which is a quad).
using render_output_event = std::vector<render_output_primitive>;

// The output from the renderer: a collection of rendered music events.
// Each element corresponds to a musical event in the input, e.g. one note.
// (Could be implicit, e.g. a bar line.)
using render_output = std::vector<render_output_event>;
}

