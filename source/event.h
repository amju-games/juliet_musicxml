#pragma once

#include "expected.hpp" // TODO <expected>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include "fraction.h"
#include "key_sig.h"

namespace juliet_musicxml
{
// Event types that we parse in MusicXML files.

// Should these types be lightweight, just the minimum info we get from
//  the xml?
// Hmm then we end up with 2 representations of basic types.

// Base class for event types. 
struct event
{
  int m_id = 0; // every event has a unique ID, the order in which it is parsed.
  int m_bar_number = 1;
  std::string m_part; // TODO index into parts - TODO
  fraction m_normalized_start_time;
  fraction m_normalized_duration; 
  std::string m_description;  // ??
  int m_staff = 0;  // ?
};

/*
enum class clef_sign
{
  CLEF_NONE,
  CLEF_TREBLE,
  CLEF_BASS,
  // TODO
};
*/

struct clef_and_line
{
  std::string m_sign;
  int m_line = 0;
};

// Clef for each staff: staff numbers are 1-based.
// map orders by stave and so is preferable to unordered_map?
using stave_num_to_clef_map = std::map<int, clef_and_line>; 

// Key sig, time sig, clef, etc
// TODO BREAK THIS UP
struct attributes : public event
{
  int m_beats = 0; // time sig: always same for all staves, in all parts?
  int m_beat_type = 0; 
 
  // If there's a clef change mid line, this map should just have the new
  //  clef(s?) - there won't necessarily be as many elements as there are 
  //  staves in the score.
  stave_num_to_clef_map m_clefs;

  // ...whereas if there's a key change, the new key sig should be rendered
  //  on all staves for this part.
  //  (Although can be rendered differently according to clef)
  //  But deffo can be different for different parts.
  key_sig m_key_sig = {};

  int m_num_staves = 1;  // Num staves for this part; I don't see this changing
    // mid piece! -- Could it?!

  // How many lines in a staff! Could be 1 for percussion.
  // I would be surprised if this changed mid-piece but who knows!
  int m_num_staff_lines = 5;
};

// Number of time units in a crotchet/quarter note.
// Can be different across parts and can change within a part!
struct divisions : public event   
{
  int m_num_divisions = 0; 
};

enum class note_stem
{
  STEM_NONE,
  STEM_UP,
  STEM_DOWN
};

// Note info -- Music XML has an 'is rest' flag within notes but would be
//  better to have a separate rest type imo

struct note_rest_base : public event // Not for polymorphism, to share common data members
{
  int m_duration = 0; // MusicXML only: Raw duration, in units of prevailing division value.
  std::string m_type; // e.g. "quarter": is this redundant? 
    // TODO If not, this should definitely be an enum, not a string

  int m_voice = 1; 
  int m_num_dots = 0;

  // TODO Articulation etc
};

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

  expected_midi_pitch calc_midi_pitch() const;

  std::string to_string() const; // get readable pitch name for info
};

struct note : public note_rest_base
{
  pitch m_pitch;

  bool m_is_chord = false; // Is this note part of a chord?
    // The first note in the chord does not get this flag set, but subsequent
    // notes in the chord do.

  note_stem m_stem;
};

struct rest : public note_rest_base
{
  int m_staff_line = 0;  // Zero means default, else position is specified
  bool m_is_whole_bar = false; 
};

// Music XML 'backup' element, to reposition the 'current time pointer'
struct backup : public event 
{
  int m_duration = 0; // The number of divisions the current time  moves backward
};

// Represents a <forward> element (moves time pointer forward).
struct forward : public event 
{
  int m_duration = 0; // Number of divisions the pointer moves forward
};

// Variant (i.e. union) of event types 
using event_union = std::variant<attributes, note, rest, backup, forward, divisions>;

// Vector of events
using event_vec = std::vector<event_union>;
}

