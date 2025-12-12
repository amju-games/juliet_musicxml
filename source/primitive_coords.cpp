#include <array>
#include "primitive_coords.h"

namespace juliet_musicxml
{
// Get the vertical spacing between the bottom line of stave i and the
//  top line of stave i+1, within the given part.
float get_stave_spacing(int part_index, int stave_num_zb)
{
  return 6; // TODO
}

// Get y-coord of top line of the given stave in the given part.
float get_stave_top(int part_index, int stave_num_zb)
{
  float y = 0;
  for (int i = 0; i < part_index; i++)
  {
    // Add up the heights of all preceding parts, including vertical
    //  distances between the parts.         
    // TODO
  }

  // Add height of each preceding stave in this part, and the space 
  //  between each adjacent stave pair.
  for (int i = 0; i < stave_num_zb; i++)
  {
    y += 4; // the height of each preceding stave -- even one-line staves
    y += get_stave_spacing(part_index, i); 
  }

  // If this is a one-line stave, we add 2 to the y-coord so we're on  
  //  the middle line of the notional 5-line space.
  // TODO

  return y;
}
}

