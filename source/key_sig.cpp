#include "key_sig.h"

namespace juliet_musicxml
{
// Description string, not final output string.
std::string to_string(key_sig ks)
{
  int sharps = static_cast<int>(ks);
  if (sharps < 8)
  {
    return std::to_string(sharps) + " sharp" + (sharps != 1 ? "s" : "");
  }
  int flats = sharps - 8;
  return std::to_string(flats) + " flat" + (flats != 1 ? "s" : "");
}
}

