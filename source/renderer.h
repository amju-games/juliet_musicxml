#pragma once

#include <unordered_map>
#include "i_renderer.h"
#include "render_output.h"

namespace juliet_musicxml
{
// New design
// render_output is built by renderer -- it's not a text_renderer, because the 
//  output isn't text.
// can be transformed by a transformer type or function, e.g. to scale coords, reverse y-coords.
// turned into text by printer type/function

// Build render_output from events
class renderer : public i_renderer
{
public:
  void render_note(const note& n) override;
  void render_rest(const rest& r) override;
  void render_vertical(const vertical& v) override;
  void render_chord(const chord& c) override;
  void render_stave(const stave_event& s) override;
  void render_clef(const clef_event& c) override;
  void render_time_sig(const time_sig_event& t) override;
  void render_key_sig(const key_sig_event& k) override;
  void render_bar_line(const bar_line_event&) override;

  render_output& output() { return m_output; } 

private:
  void add_output_event(const render_output_event&);
  void add_description_to_output(const std::string& description);
  void set_clef_for_stave(int part_index, int stave_num, clef_sign cl);
  clef_sign get_clef_for_stave(int part_index, int stave_num) const;
  void add_stave_info(int part_index, int num_staves, int num_stave_lines);

private:
  render_output m_output;
  float m_x_offset = 0;
  [[maybe_unused]]float m_y_offset = 0;
  std::unordered_map<int, clef_sign> m_clef_map; // store current clef for each stave

  // TODO
  // page_info m_page_info; // This is the const, user-defined params for the page.

  // scratchpad m_scratch; // info built on fly to track current offsets, staves/clefs, etc.
};
}

