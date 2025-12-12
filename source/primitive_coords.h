#pragma once

#include "event.h"

namespace juliet_musicxml
{
// Calc y-position of given note on the given clef.
float calc_note_y_pos(const note& n, clef_sign clef);

// Get y-coord of top line of the given stave in the given part.
float get_stave_top(int part_index, int stave_num_zb);

// Get distance in y between staves
float get_stave_spacing(int part_index, int stave_num_zb);
}

