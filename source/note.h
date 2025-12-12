#pragma once

#include "event.h"

namespace juliet_musicxml
{
struct ledger_lines
{
  float width = 0; // decided according to note type, and chord members.
  // Examples:
  //  -o-   -- one minim, crotchet, or [semi-*]quaver
  //  -()-   -- one seimbreve
  //  -o--
  //    o   -- two notes on adjacent stave positions
  //  -()--
  //    ()  -- two seimbreves on adjacent stave positions

  // Num lines above and below stave - for a single note, only
  //  one of these will be non-zero, but for a chord, they could
  //  both be non-zero.
  int m_num_ledger_lines_above = 0;
  int m_num_ledger_lines_below = 0;
}; 

struct note : public note_rest_base
{
  pitch m_pitch;

  bool m_is_chord = false; // Is this note part of a chord?
    // The first note in the chord does not get this flag set, but subsequent
    // notes in the chord do.

  stem m_stem;

  // Calculated in logically const render pass
  mutable ledger_lines m_ledger_lines;

  // TODO Articulation etc

  void normalize_time(int& ticks, int& divisions, fraction& time) override;
  float get_width() const override;
  std::string get_description() const override;
  void render(i_renderer&) const override;

  // Pre-render calculations 
  // Calc ledger lines for this note when rendered on the given clef
  void pre_render_ledger_lines(clef_sign clef) const; // logically const
  float calc_note_y_pos(clef_sign clef) const;
};
}

