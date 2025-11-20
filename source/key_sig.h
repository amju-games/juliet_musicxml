#pragma once

#include <string>

namespace juliet_musicxml
{
enum class key_sig
{
  KEYSIG_0_SHARP,
  KEYSIG_1_SHARP,
  KEYSIG_2_SHARP,
  KEYSIG_3_SHARP,
  KEYSIG_4_SHARP,
  KEYSIG_5_SHARP,
  KEYSIG_6_SHARP,
  KEYSIG_7_SHARP,

  KEYSIG_0_FLAT,
  KEYSIG_1_FLAT,
  KEYSIG_2_FLAT,
  KEYSIG_3_FLAT,
  KEYSIG_4_FLAT,
  KEYSIG_5_FLAT,
  KEYSIG_6_FLAT,
  KEYSIG_7_FLAT,
};

std::string to_string(key_sig ks);
}

