#pragma once

#include "score.h"

void print_events(const juliet_musicxml::event_vec& events);

void print_bars(const juliet_musicxml::bar_vec& bars);

void print_score(const juliet_musicxml::score& s);

bool parse_and_print(const std::string& filename);

