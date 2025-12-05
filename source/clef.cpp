#include "clef.h"

namespace juliet_musicxml
{
std::optional<std::string> to_string(clef_sign c)
{
  static const std::map<clef_sign, std::string> CLEF_STR
  {   
    { clef_sign::CLEF_NONE,   "BAD_CLEF" },
    { clef_sign::CLEF_TREBLE, "treble" },
    { clef_sign::CLEF_BASS,   "bass" },
    { clef_sign::CLEF_ALTO,   "alto" },
    { clef_sign::CLEF_TENOR,  "tenor" },
  };  

  if (!CLEF_STR.contains(c))
  {
    return std::nullopt;
  }

  return CLEF_STR.at(c);
}

clef_sign get_clef_sign(const clef_and_line& cl)
{
  if (cl.m_sign == "F") return clef_sign::CLEF_BASS;

  if (cl.m_sign == "G") 
  {
    if (cl.m_line == 2)
    {
      if (cl.m_octave_change == -1) return clef_sign::CLEF_VOCAL_TENOR;
      return clef_sign::CLEF_TREBLE;
    }
  }

  if (cl.m_sign == "C") 
  {
    if (cl.m_line == 3) return clef_sign::CLEF_ALTO;
    if (cl.m_line == 4) return clef_sign::CLEF_TENOR;
  }
  
  return clef_sign::CLEF_NONE;
}
}

