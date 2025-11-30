#include "event.h"

namespace juliet_musicxml
{
struct test_event : public event
{
  test_event(const std::string& description, const fraction& start_time, int voice = 1) : 
    m_description(description), m_voice(voice)
  {
    m_normalized_start_time = start_time;
  }

  std::string get_description() const override 
  {
    return m_description;
  }

  void render(i_renderer&) const override
  {
  }

  std::string m_description;
  int m_voice = 1;
};
}

