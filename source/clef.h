#pragma once

#include <map>
#include <optional>
#include <string>

namespace juliet_musicxml
{
enum class clef_sign
{
  CLEF_NONE,
  CLEF_TREBLE,
  CLEF_BASS,
  CLEF_ALTO,
  CLEF_TENOR,
  CLEF_VOCAL_TENOR,
  // TODO any more?
};

std::optional<std::string> to_string(clef_sign);

// MusicXML only: clef is represented by a sign and stave line.
struct clef_and_line
{
  std::string m_sign;
  int m_line = 0;
  int m_octave_change = 0; // -1 for vocal tenor clef
};

// Convert MusicXML sign+line into clef_sign enum.
clef_sign get_clef_sign(const clef_and_line&);

// Clef for each stave: stave numbers are 1-based in MusicXML 
//  * but zero based here! *
// map orders by stave and so is preferable to unordered_map.
using stave_num_to_clef_map = std::map<int, clef_sign>; 
}

