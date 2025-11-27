#pragma once

#include "vertical.h"

namespace juliet_musicxml
{
struct chord_info
{
  // Ledger lines for chord, supersedes ledger lines for individual notes.
  ledger_lines m_above, m_below; // ..?

  // Stem is union of all stems of notes in chord, adjusted for beams
  stem m_stem;
};

// Not sold on this design - chords are verticals?? Maybe a different composite?
struct chord : public vertical
{
  // Construct chord by moving a range of events
  chord(event_vec::iterator first, event_vec::iterator last) : vertical(first, last) {}

  std::string get_description() const override;

  chord_info m_chord_info;
};
}

