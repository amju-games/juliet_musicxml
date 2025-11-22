#pragma once

#include "score.h"

// Helper functions for testing snippets of xml

namespace juliet_musicxml
{
// Parse the contents of a <measure>, giving the result as a score.
score parse_score(const std::string& xml_snippet);
 
// Parse the contents of a <measure>, giving the result as a bar.
bar parse_bar(const std::string& xml_snippet);

// Parse the contents of a <measure>, giving the result as a single event.
p_event parse_event(const std::string& xml_snippet);
}

