#include <unordered_map>
#include <ostream>
#include <sstream>
#include "chord.h"
#include "event.h"
#include "glyph_strings.h"
#include "note.h"
#include "primitive_coords.h"
#include "renderer.h"
#include "vertical.h"

namespace juliet_musicxml
{
namespace internal
{
std::string get_rest_glyph_name(const rest& r)
{
  duration d = r.m_duration_type;
  // Look up from internal duration enum to final output string.
  static const std::unordered_map<duration, std::string> REST_GLYPHS
  {
    { duration::DUR_MAXIMA, "maxima-rest" },
    { duration::DUR_LONG, "long-rest" },
    { duration::DUR_BREVE, "breve-rest" },
    { duration::DUR_WHOLE, "semibreve-rest" },
    { duration::DUR_HALF,  "minim-rest" },
    { duration::DUR_QUARTER, "crotchet-rest" },
    { duration::DUR_8TH, "quaver-rest" },
    { duration::DUR_16TH, "semiquaver-rest" },
    // Not supported beyond a certain point
    { duration::DUR_32ND, "32nd-rest" },
    { duration::DUR_64TH, "64th-rest" },
    { duration::DUR_128TH, "128th-rest" },
    { duration::DUR_256TH, "256th-rest" },
    { duration::DUR_512TH, "512th-rest" },
    { duration::DUR_1024TH, "1024th-rest" }
    // etc
  };
  assert(REST_GLYPHS.contains(d));
  return REST_GLYPHS.at(d);
}

std::string get_note_head_glyph_name(const note& n)
{
  duration d = n.m_duration_type;
  // Look up note head glyph name for duration. Anything less than a crotchet
  //  uses the same note head -- but this could be overridden by the xml.
  if (static_cast<int>(d) >= static_cast<int>(duration::DUR_QUARTER))
  {
    return glyph::NOTE_HEAD_CROTCHET;
  }

  static const std::unordered_map<duration, std::string> NOTE_HEAD_GLYPHS
  {
    { duration::DUR_MAXIMA, "TODO" },
    { duration::DUR_LONG,   "TODO" },
    { duration::DUR_BREVE,  "TODO" },
    { duration::DUR_WHOLE,  glyph::NOTE_SEMIBREVE },
    { duration::DUR_HALF,   glyph::NOTE_HEAD_MINIM }
  };
  assert(NOTE_HEAD_GLYPHS.contains(d));
  return NOTE_HEAD_GLYPHS.at(d);
}

auto make_quad(x_coord min_x, y_coord top_y, x_coord max_x, y_coord bottom_y)
{
  // Make axis-aligned quad
  return quad_output{{ // Argh std::arrays require TWO {{ }}, 
                       //  sure hope they fix that in c++26 sigh
    { min_x, top_y },
    { max_x, top_y },
    { max_x, bottom_y },
    { min_x, bottom_y }}};
}

float get_line_width(const bar_line_event& bl) 
{
  return 0.1f;
}

int get_num_staves(int part_index)
{
  return 2;
}

//int get_num_stave_lines(int part_index, int stave_num_zb)
//{
//  // Retrieve info set by stave event
//  // TODO
//  return 5;
//}

/*
Stave vertical distances
========================

One five line stave is 4 units high, by our definition of distance units.
 (Plus the height of one stave line, which we can ignore, I think.)

For a one-line stave (percussion/just beats), the middle line is drawn, 
  and we keep the stave height as 4, because we want to draw bar lines, time
  sigs, repeats etc as for a 5-line stave.

Within a part, there is a distance between each stave in that part.
Between parts, there is another distance. 
These distances can be calculated per-line, to avoid overlaps, so are
 not consts.

There is also the distance between lines, which is the current y-offset.
For the top line of the page, the y-offset lowers the top stave so notes
don't go off the top of the page; also we may have title, composer etc text.

      ---------------- <stave n-1>
 |    ----------------
 4    ----------------
 |    ----------------
      ----------------
    
      y-distance between bottom of stave n-1 and top of stave n in this part

      ---------------- <stave n>
 |    ----------------
 4    ----------------
 |    ----------------
      ----------------
     
      y-distance between part m-1 and m

      <part m> 

NB Annoyingly, MusicXML stave numbers are one-based, but at this point,
 stave numbers are zero-based, from the top stave for the part.
*/

float get_stave_bottom(int part_index, int stave_num_zb)
{
  // Height of a stave is 4 -- even a one-line stave.
  return get_stave_top(part_index, stave_num_zb) + 4.f;
}

float event_x(const event& e)
{
  return 2.0f * static_cast<float>(e.m_id);
}

auto get_glyph_name_for_int(int i)
{
  return std::to_string(i); // Hmm but I think we should be using codes
  // for glyphs throughout, only converting to strings when we output to
  // ostream.
}

int munge(int a, int b)
{
  return (a << 8) | b;
}

// TODO Use glyph codes
std::optional<std::string> get_glyph_name_for_key_on_clef(key_sig ks, clef_sign clef)
{
  if (ks == key_sig::KEYSIG_0_SHARP || ks == key_sig::KEYSIG_0_FLAT)
  {
    return ""; 
  }

  std::string res = "keysig-" + to_string(ks);

  auto clef_str = to_string(clef);
  if (!clef_str)
  {
    return std::nullopt;
  }

  res += clef_str.value();
  return res;
}

std::string get_glyph_name_for_clef(clef_sign sign)
{
  auto result = to_string(sign);
  if (!result)
  {
    return "BAD_CLEF";
  }
  return result.value() + "-clef";  
}

void add_note_head_to_primitives(
  const note& n, render_output_event& primitives, clef_sign clef)
{
  float x = event_x(n); // Adjust if required
  float y = n.calc_note_y_pos(clef);

  primitives.emplace_back(glyph_output{get_note_head_glyph_name(n), {x, y}});  
  // Dots! Articulations!
}

float get_stem_width()
{
  // For all notes
  return 0.1f; // TODO config
}

float get_stem_height()
{
  // Big TODO
  return 3.f;
}
  
void add_ledger_lines_for_note(
  const note& n, render_output_event& primitives, clef_sign clef)
{
}

void add_stem_for_note(
  const note& n, render_output_event& primitives, clef_sign clef)
{
  // Does this note need a stem at all?
  if (static_cast<int>(n.m_duration_type) <= static_cast<int>(duration::DUR_WHOLE))
  {
    return;
  }
  
  float x = event_x(n); // Adjust if required
  float y = n.calc_note_y_pos(clef);
  float w = get_stem_width();
  float h = get_stem_height(); // phew some heavy lifting in here
   
  // Up or down? -- up = x offset
  switch (n.m_stem.m_direction)
  {
  case stem::direction::STEM_DOUBLE:
    // Unsupported
    break;
  case stem::direction::STEM_NONE:
    // Hmm I guess we don't need the above duration test?
    // Maybe this enum value has other uses..?
    return;
  case stem::direction::STEM_UP:
    x += n.get_width() - w;
    h = -h;
    break;
  case stem::direction::STEM_DOWN:
    break;
  };

  primitives.emplace_back(make_quad(x, y, x + w, y + h));
} 
} // namespace internal

void renderer::render_stave(const stave_event& s)
{
  // Ah nice, this is how we know about the staves for all the parts.
  // Or maybe we already got this info and stashed it in a horizontal?

  add_stave_info(s.m_part_index, s.m_stave_info);
}

void renderer::render_staves()
{
  render_output_event primitives;

  /*
  // Description
  std::stringstream ss;
  ss << "Part " << s.m_part_index << " num staves: " << s.m_num_staves << " num lines: " << s.m_num_stave_lines;
  primitives.emplace_back(description_output(ss.str()));

  // Render each stave
  for (int i = 0; i < s.m_num_staves; i++)
  {
    primitives.emplace_back(description_output(" - stave " + std::to_string(i)));
    float x = 0; // TODO event_x(k);
    float y = get_stave_top(s.m_part_index, i); 
    float w = 20.f; // TODO just a test, this should be the full width of the stave
    float h = 0.1f; //get_stave_line_width();

    // TODO 
    for (int j = 0; j < s.m_num_stave_lines; j++)
    {
      primitives.emplace_back(make_quad(x, y, x + w, y + h));
      y += 1.0f; // By definition, stave lines are 1 unit apart
    }
    //y += get_stave_spacing(...);
  }
  */
  add_output_event(primitives); 
}

void renderer::add_stave_info(int part_index, const stave_info& s)
{
  m_stave_info_map[part_index] = s;
}

std::string get_description_string_for_clef(clef_sign clef)
{
  auto opt_clef_str = to_string(clef);
  std::string clef_str = " Clef: ";
  if (opt_clef_str)
  {
    clef_str += opt_clef_str.value();
  }
  else
  {
    std::stringstream ss;
    ss << "\nError: no clef";
    clef_str += ss.str();
  }
  return clef_str;
}

void renderer::note_pre_render(const note& n, clef_sign clef) 
{
  // Should this live in note? Yes, for testability.
  n.pre_render_ledger_lines(clef);
}

void renderer::render_note(const note& n) 
{
  render_output_event primitives;

  // Get clef, required for note y-position
  auto clef = get_clef_for_stave(n.m_part_index, n.m_stave);

  note_pre_render(n, clef);

  // Output comment, including clef
  auto clef_str = get_description_string_for_clef(clef);
  primitives.emplace_back(description_output(n.get_description() + clef_str));

  // TODO needs y origin value. Use top line of stave 0. If a note is on
  //  another stave, add the appropriate spacing etc. This will work
  //  with chords across staves.
  internal::add_note_head_to_primitives(n, primitives, clef);

  // TODO Stems, ledger lines... accidentals here too, positions
  //  could change in a chord.

  // TODO Use same y origin.
  internal::add_stem_for_note(n, primitives, clef);

  // TODO Same y origin. NB if space between staves is always an integer,
  //  we can use ints for ledger line positions which might help simplify
  //  chord ledger lines?
  internal::add_ledger_lines_for_note(n, primitives, clef);

  add_output_event(primitives); 
}

void renderer::render_rest(const rest& r) 
{
  render_output_event primitives;

  // Get the x offset, and y-coord of line 1 of the stave this thing goes on.
  // TODO Make sure we have the stave number for all events, and a way
  //  to get the y-offset from it. (I.e., what's the accumulated y-offset of 
  //  all the staves in all the parts. We will need to refer to the part info,
  //  in the original score. That's where we have stave info for each part.)
  coord pos {}; //{m_x_offset, calc_base_y(r)}; 

  // TODO Adjust y coord so the rest sits in the vertical centre of the stave,
  //  or in a custom y-pos if that was specified.

  primitives.emplace_back(glyph_output{internal::get_rest_glyph_name(r), pos});  
  //add_dots(r, pos, primitives); // Add dot glyphs as necessary. Same as for notes.
  add_output_event(primitives); 
}

void renderer::render_vertical(const vertical& v) 
{
  // Set the current x-offset for all children of the current vertical.
  m_x_offset = v.m_info.m_x_offset;
  // ....render_horizontal (it's coming) would reset that x-offset to zero.
  // AND store the y-offset.

  // Perhaps when we render a vertical would be a good place
  //  to add a TIME output string, for animation - if we know the duration
  //  or end time of the vertical (we should do, right?)
  // But we only want to animate notes and rests, I think? Not clefs etc.
  // We want an m_is_animated flag in events.
  // Timeslice is the normalized start time scaled by the length of the entire
  //  piece, so in the range 0..1.
  // NB We will have to calc the timeslice by doing a pass over the whole
  //  score, to get the total normalized time length.
  // When we animate, we would want to know that info so we know how fast
  //  to increase the animation parameter from 0..1.
//  render_output_event primitives;
//  primitives.emplace_back(time_output{ v.m_timeslice_start, v.m_timeslice_end});  
//  add_output_event(primitives); 
}

void renderer::render_chord(const chord& c) 
{
  render_output_event primitives;

  // Debug description for chord
  primitives.emplace_back(description_output(c.get_description()));

  // Get stem, ledger lines, etc from note children, draw these separately,
  //  because we don't want to overlap stems etc.
  // We can then draw the note heads, beams, etc. as for notes.
  // We _don't_ draw the children directly as we do in render_vertical.

  for (const auto& ch : c.m_children)
  {
    note* n = dynamic_cast<note*>(ch.get());
    assert(n);

    auto clef = get_clef_for_stave(n->m_part_index, n->m_stave);

    // Calc ledger lines, stems, etc
    note_pre_render(*n, clef);

    internal::add_note_head_to_primitives(*n, primitives, clef);
  }

  // TODO Stems, ledger lines... accidentals here too, positions
  //  could change in a chord.

  add_output_event(primitives); 
}

void renderer::set_clef_for_stave(int part_index, int stave_num, clef_sign cl)
{
  m_clef_map[internal::munge(part_index, stave_num)] = cl;
}

clef_sign renderer::get_clef_for_stave(int part_index, int stave_num) const
{
  int key = internal::munge(part_index, stave_num); 
  if (!m_clef_map.contains(key))
  {
    return clef_sign::CLEF_TREBLE; // TODO Check this default value is valid
  }
  return m_clef_map.at(key);
}

void renderer::render_clef(const clef_event& c) 
{
  // Output a glyph for each clef in the event
  // Also, store the latest clef for each part+stave.

  render_output_event primitives;

  primitives.emplace_back(description_output(c.get_description()));

  for (const auto& [stave_num, sign] : c.m_clef_map)
  {
    if (sign == clef_sign::CLEF_NONE)
    {
      continue; 
    }

    // TODO If a cue clef, scale the glyph 

    float x = internal::event_x(c);
    float y = get_stave_top(c.m_part_index, stave_num); 
    // TODO y offset? Depends on clef sign??

    primitives.emplace_back(glyph_output{internal::get_glyph_name_for_clef(sign), {x, y}});

    // Store clef info for later use.
    set_clef_for_stave(c.m_part_index, stave_num, sign); 
  }
  add_output_event(primitives); 
}

void renderer::render_time_sig(const time_sig_event& t) 
{
  // We render the time sig on all staves in the part.

  render_output_event primitives;

  // Debug description for time sig
  primitives.emplace_back(description_output(t.get_description()));

  // For each stave in this part
  for (int i = 0; i < internal::get_num_staves(t.m_part_index); i++)
  { 
    // Two number glyphs, vertically aligned
   
    float x = internal::event_x(t);
    float y = get_stave_top(t.m_part_index, i); // TODO y-offset for glyph anchor?  

    // Top number
    primitives.emplace_back(glyph_output{ 
      internal::get_glyph_name_for_int(t.m_fraction.num), {x, y}});
   
    // Bottom number -- add 2 to y coord (unit is distance between staves).
    primitives.emplace_back(glyph_output{ 
      internal::get_glyph_name_for_int(t.m_fraction.denom), {x, y + 2.0f}});
  }
  add_output_event(primitives); 
}

void renderer::render_key_sig(const key_sig_event& k) 
{
  // As for time sig, we draw this on all staves. But interestingly,
  //  what we draw depends on the currently prevailing clef for each
  //  stave. 

  render_output_event primitives;

  // Debug description for time sig
  primitives.emplace_back(description_output(k.get_description()));

  // For each stave in this part
  for (int i = 0; i < internal::get_num_staves(k.m_part_index); i++)
  { 
    float x = internal::event_x(k);
    float y = get_stave_top(k.m_part_index, i); 

    // Get the prevailing clef for this stave
    auto clef = get_clef_for_stave(k.m_part_index, i);

    // TODO we should probably check that optional. If it fails,
    //  we probably don't know the clef for this stave. In that
    //  case, let's bail out and not try to render any key sig.
    auto clef_glyph = internal::get_glyph_name_for_key_on_clef(k.m_key_sig, clef);
    if (!clef_glyph)
    {
      return;
    }
    primitives.emplace_back(glyph_output{clef_glyph.value(), {x, y}});
  }
  add_output_event(primitives); 
}

void renderer::render_bar_line(const bar_line_event& bl) 
{
  using namespace internal;

  // For a simple bar line, just add a line(i.e. quad) primitive.
  render_output_event primitives;
  // Make a line from top of stave 0 to bottom of the last stave for
  //  the part to which this event belongs.
  // TODO Make sure x-offset is lined up with the right hand edge of the stave!
  primitives.emplace_back(make_quad(
    event_x(bl), 
    get_stave_top(bl.m_part_index, 0 /* stave index */),
    event_x(bl) + get_line_width(bl),
    get_stave_bottom(bl.m_part_index, get_num_staves(bl.m_part_index) - 1)));
    
  // Hmm what about a stave with one line - the bar lines should extend up
  //  and down, perhaps as if there were 5 lines with the single line 
  //  in the middle. TODO

  add_output_event(primitives); 
}

void renderer::add_description_to_output(const std::string& d)
{
  render_output_event r{description_output(d)};
  add_output_event(r);
}

void renderer::add_output_event(const render_output_event& e)
{
  m_output.emplace_back(e);
}
}

