#pragma once

#include "expected.hpp" // TODO <expected>
#include <string>

namespace juliet_musicxml
{
// Pitch event type that we parse in MusicXML files.
// Octave 4 is the octave started by Middle C.

/*

  |    |    |    | |    |   |    |    | |    | |    |   |
  |    |    | 61 | | 63 |   |    | 66 | | 68 | | 70 |   |    ...
__|    |    |____| |____|   |    |____| |____| |____|   |      
|      |      |      |      |      |      |      |      |      |
|  B3  |  C4  |  D4  |  E4  |  F4  |  G4  |  A4  |  B4  |  C5  |
|      |Middle|      |      |      |      |440Hz |      |      |
|      |  C   |      |      |      |      |      |      |      |
|  59  |  60  |  62  |  64  |  65  |  67  |  69  |  71  |  72  |
|______|______|______|______|______|______|______|______|______|__

*/

enum class pitch_error
{
  PITCH_TOO_LOW,
  PITCH_TOO_HIGH,
  PITCH_BAD_STEP,
};

// MIDI pitch (C4 (Middle C) = 60), or error code
using expected_midi_pitch = tl::expected<int, pitch_error>;

struct pitch
{
  // In MusicXML, step/octave/alter together define the pitch of the note.
  // This is more useful than simple MIDI pitch, as it defines the stave
  //  position for the note.
  char m_step = '\0'; // A, a, B, b, etc. 
  int m_octave = 0;   // Octave 4 is octave started by middle C.
  int m_alter = 0;    // -1 for flat, +1 for sharp etc. 
    // TODO Also can be fractional for microtones, not supporting this.

  int m_midi_pitch = 60; // Calculated once we have all the xml elements parsed.

  expected_midi_pitch calc_midi_pitch() const;

  std::string to_string() const; // get readable pitch name for info
};
}

