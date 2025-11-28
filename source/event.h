#pragma once

#include "expected.hpp" // TODO <expected>
#include <map>
#include <string>
#include <vector>
#include "fraction.h"
#include "key_sig.h"
#include "pitch.h"

namespace juliet_musicxml
{
// Event types that we parse in MusicXML files.

// Base class for event types. 
struct event
{
  virtual ~event() = default;

  // MusicXML events don't know their start time until we do the 
  //  time normalization pass, where we count up durations, and use
  //  backup/forward/divisions events.
  virtual void normalize_time(int& ticks, int& divisions, fraction& time);

  // Create description on the fly, because we add more data with each pass.
  virtual std::string get_description() const { return {}; }
  
  void set_id(int id) { m_id = id; }
 
  int m_id = 0; // every event has a unique ID, the order in which it is parsed.
  int m_bar_number = 1;
  std::string m_part; // TODO index into parts - TODO
  fraction m_normalized_start_time;
  fraction m_normalized_duration; 
  int m_staff = 0;  // ?
  bool m_is_renderable = true;
  bool m_is_attribute = false;
};

struct non_renderable_event : public event
{
  non_renderable_event() { m_is_renderable = false; }
};

// Superclass for clefs, time sigs, etc
struct attribute_event : public event
{
  attribute_event() { m_is_attribute = true; }
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

struct time_sig_event : public attribute_event
{
  fraction m_fraction; // what kind of weird time sigs are there...?
  std::string get_description() const override;
};

struct key_sig_event : public attribute_event
{
  key_sig m_key_sig = {};
  std::string get_description() const override;

  // Set from the MusicXML 'fifths' key sig attribute, where positive means number of
  //  sharps, negative means number of flats.
  // Return true if set successfully.
  bool set_from_num_fifths(int num_fifths);
};

struct clef_event : public attribute_event
{
  stave_num_to_clef_map m_clef_map;
  std::string get_description() const override;
};

// Hmmm this info goes in the info for the current part we are parsing...
//  but it's not renderable in a vertical. Riiight?
struct stave_event : public non_renderable_event /* not attribute_event?! */
{
  int m_num_staves = 1;
  int m_num_staff_lines = 5;
  std::string get_description() const override;
};

// Number of time units in a crotchet/quarter note.
// Can be different across parts and can change within a part!
struct divisions : public non_renderable_event   
{
  int m_num_divisions = 0; 

  void normalize_time(int& ticks, int& divisions, fraction& time) override;
  std::string get_description() const override;
};

struct stem
{
  enum class direction
  {
    STEM_NONE, // for semibreves, for example
    STEM_UP,
    STEM_DOWN
  };

  std::string to_string() const;

  direction m_direction;
  float m_length = 1.0f; // stem length for a standard crotchet
};

// Note info -- Music XML has an 'is rest' flag within notes but would be
//  better to have a separate rest type imo

struct note_rest_base : public event 
{
  int m_duration = 0; // MusicXML only: Raw duration, in units of prevailing division value.
  std::string m_type; // e.g. "quarter": is this redundant? 
    // TODO If not, this should definitely be an enum, not a string

  int m_voice = 1; 
  int m_num_dots = 0;

  // TODO Articulation etc
};

struct ledger_lines
{
  enum class direction { ABOVE, BELOW };
  int m_num_ledger_lines = 0;
  float m_width = 1.0f; // whatever that means - wider for semibreves, etc.
  direction m_direction;
}; 

struct note : public note_rest_base
{
  pitch m_pitch;

  bool m_is_chord = false; // Is this note part of a chord?
    // The first note in the chord does not get this flag set, but subsequent
    // notes in the chord do.

  stem m_stem;

  ledger_lines m_ledger_lines;

  void normalize_time(int& ticks, int& divisions, fraction& time) override;
  std::string get_description() const override;
};

struct rest : public note_rest_base
{
  int m_staff_line = 0;  // Zero means default, else position is specified
  bool m_is_whole_bar = false; 

  void normalize_time(int& ticks, int& divisions, fraction& time) override;
  std::string get_description() const override;
};

// Music XML <backup> element, to reposition the 'current time pointer'
struct backup : public non_renderable_event 
{
  int m_duration = 0; // The number of divisions the current time  moves backward

  void normalize_time(int& ticks, int& divisions, fraction& time) override;
  std::string get_description() const override;
};

// MusicXML <forward> element (moves time pointer forward).
struct forward : public non_renderable_event 
{
  int m_duration = 0; // Number of divisions the pointer moves forward

  void normalize_time(int& ticks, int& divisions, fraction& time) override;
  std::string get_description() const override;
};

using p_event = std::unique_ptr<event>;

using event_vec = std::vector<p_event>;
}

