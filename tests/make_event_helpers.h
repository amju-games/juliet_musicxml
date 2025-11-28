#include "event.h"

// Simplify creating notes for tests
std::unique_ptr<juliet_musicxml::note> make_note(std::string_view step, int time, int voice);

std::unique_ptr<juliet_musicxml::rest> make_rest(int time, int voice = 1);

std::unique_ptr<juliet_musicxml::clef_event> make_clef(int time, std::string sign); 

