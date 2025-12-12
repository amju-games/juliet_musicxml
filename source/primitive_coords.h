#pragma once

#include "event.h"

namespace juliet_musicxml
{
struct note;

// Get y-coord of top line of the given stave in the given part.
float get_stave_top(int part_index, int stave_num_zb);

// Get distance in y between staves
float get_stave_spacing(int part_index, int stave_num_zb);
}

