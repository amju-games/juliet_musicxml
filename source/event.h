#pragma once

#include "duration.h"
#include "expected.hpp" // TODO <expected>
#include <map>
#include <string>
#include <vector>
#include "fraction.h"
#include "key_sig.h"
#include "pitch.h"

namespace juliet_musicxml
{
class i_renderer;

// Event types that we parse in MusicXML files.

// Base class for event types. 
struct event
{
  virtual ~event() = default;

  virtual void render(i_renderer& r) const = 0;

  // MusicXML events don't know their start time until we do the 
  //  time normalization pass, where we count up durations, and use
  //  backup/forward/divisions events.
  virtual void normalize_time(int& ticks, int& divisions, fraction& time);

  // Get width of this element; for composite elements, the max or accumulated
  //  width. 
  // Distance units: 1.0 == distance between two adjacent stave lines.
  virtual float get_width() const = 0;

  // Create description on the fly, because we add more data with each pass.
  virtual std::string get_description() const = 0;
  
  void set_id(int id) { m_id = id; }
 
  int m_id = 0; // every event has a unique ID, the order in which it is parsed.
  int m_bar_number = 1;
  int m_part_index = 0; // index into parts in score
  fraction m_normalized_start_time;
  fraction m_normalized_duration; 
  bool m_is_renderable = true;
  bool m_is_attribute = false;
};

using p_event = std::unique_ptr<event>;

using event_vec = std::vector<p_event>;

struct non_renderable_event : public event
{
  non_renderable_event() { m_is_renderable = false; }
  void render(i_renderer&) const override {}
  float get_width() const override { return 0; } 
};

// Superclass for clefs, time sigs, etc
struct attribute_event : public event
{
  attribute_event() { m_is_attribute = true; }
};

struct composite_event : public event
{
  void render(i_renderer&) const override;
  float get_width() const override;

  event_vec m_children;
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
  float get_width() const override;
  void render(i_renderer&) const override;
};

struct key_sig_event : public attribute_event
{
  key_sig m_key_sig = {};

  float get_width() const override;
  std::string get_description() const override;
  void render(i_renderer&) const override;

  // Set from the MusicXML 'fifths' key sig attribute, where positive means number of
  //  sharps, negative means number of flats.
  // Return true if set successfully.
  bool set_from_num_fifths(int num_fifths);
};

struct clef_event : public attribute_event
{
  stave_num_to_clef_map m_clef_map;

  float get_width() const override;
  std::string get_description() const override;
  void render(i_renderer&) const override;
};

// Hmmm this info goes in the info for the current part we are parsing...
//  but it's not renderable in a vertical. Riiight?
// When we 'render' this, we pass info to the renderer, but we don't 
//  directly write output for it.
struct stave_event : public attribute_event
{
  int m_num_staves = 1;
  int m_num_staff_lines = 5;
  float get_width() const override { return 0; }
  std::string get_description() const override;
  void render(i_renderer&) const override;
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
  // Enum value for xml 'type' string, e.g. "quarter".
  // This is used for deciding the glyph for the note or rest.
  duration m_duration_type; 

  int m_staff = 0;  // Used with part index to calc position for rendering

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
  float get_width() const override;
  std::string get_description() const override;
  void render(i_renderer&) const override;
};

struct rest : public note_rest_base
{
  int m_staff_line = 0;  // Zero means default, else position is specified
    // TODO Might need a separate flag, in case 0 is a valid position.

  bool m_is_whole_bar = false; 

  void normalize_time(int& ticks, int& divisions, fraction& time) override;
  float get_width() const override;
  std::string get_description() const override;
  void render(i_renderer&) const override;
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

// Sort vec of events, primarily by normalized start time.
void sort(event_vec& events);
}

