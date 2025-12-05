#pragma once

#include "expected.hpp" // TODO <expected>
#include <string>

namespace juliet_musicxml
{
// Pitch event type that we parse in MusicXML files.

enum class pitch_error
{
  PITCH_TOO_LOW,
  PITCH_TOO_HIGH,
  PITCH_BAD_STEP,
};

// MIDI pitch (i.e. C3 (Middle C) = 60), or error code
using expected_midi_pitch = tl::expected<int, pitch_error>;

// MusicXML only
struct pitch
{
  // Step/octave/alter together define the pitch of the note.
  char m_step = '\0'; // C, D, E, etc. 
  int m_octave = 0;    // 4, 5, etc. 
  int m_alter = 0; // -1 for flat, etc. Seems redundant but possibly useful.
    // TODO Also can be fractional for microtones, not supporting this.

  int m_midi_pitch = 60;

  expected_midi_pitch calc_midi_pitch() const;

  std::string to_string() const; // get readable pitch name for info
};
}

