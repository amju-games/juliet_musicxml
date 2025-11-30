#pragma once

namespace juliet_musicxml
{
struct note;
struct rest;
struct vertical;
struct chord;
struct clef_event;
struct time_sig_event;
struct key_sig_event;

class i_renderer 
{
public:
    virtual ~i_renderer() = default;
    
    // Core Event types (Rhythmic)
    virtual void render_note(const note& n) = 0;
    virtual void render_rest(const rest& r) = 0;
    
    // Composite/Grouping events
    virtual void render_vertical(const vertical& v) = 0;
    virtual void render_chord(const chord& c) = 0;
    
    // Attribute events
    virtual void render_clef(const clef_event& c) = 0;
    virtual void render_time_sig(const time_sig_event& t) = 0;
    virtual void render_key_sig(const key_sig_event& k) = 0;

    // ... and so on for bar_line, etc.
};
}

