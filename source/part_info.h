#pragma once

namespace juliet_musicxml
{
// Info about one part
struct part_info
{
  int m_part_index = 0; // index into vector of all parts 
  std::string m_part_id; // MusicXML unique ID for the part
  std::string m_name; // Text we render at the left edge of the first line
    // to identify this part.
    // NB Perhaps should be a more specific string type.
  std::string m_short_name; // For lines after line 1

  // Scale for all glyphs in the part. (Perhaps also height of stave line
  //  scales with this too. But width of stave wouldn't be different.)
  float m_scale = 1.0f; 
};
}

