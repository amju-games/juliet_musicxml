#include <unordered_map>
#include "duration.h"
#include "xml_strings.h"

namespace juliet_musicxml
{
tl::expected<duration, std::string> from_string(std::string xml_type)
{
  // Map MusicXML strings to enum values
  static const std::unordered_map<std::string, duration> DUR_NAMES
  {
    { xml::MAXIMA, duration::DUR_MAXIMA },
    { xml::LONG, duration::DUR_LONG },
    { xml::BREVE, duration::DUR_BREVE },
    { xml::WHOLE, duration::DUR_WHOLE },
    { xml::HALF, duration::DUR_HALF },
    { xml::QUARTER, duration::DUR_QUARTER },
    { xml::EIGHTH, duration::DUR_8TH },
    { xml::_16TH, duration::DUR_16TH },
    { xml::_32ND, duration::DUR_32ND },
    { xml::_64TH, duration::DUR_64TH },
    { xml::_128TH, duration::DUR_128TH },
    { xml::_256TH, duration::DUR_256TH },
    { xml::_512TH, duration::DUR_512TH },
    { xml::_1024TH, duration::DUR_1024TH },
  };
  if (DUR_NAMES.contains(xml_type)) ///auto it = DUR_NAMES.find(xml_type);
  {
    return DUR_NAMES.at(xml_type);
  }
  return tl::unexpected("Bad duration type string: " + xml_type);
}

std::ostream& operator<<(std::ostream& os, duration d)
{
  static const std::unordered_map<duration, std::string> DUR_NAMES
  {
    { duration::DUR_MAXIMA, "Mxma" },
    { duration::DUR_LONG, "Long" },
    { duration::DUR_BREVE, "Brev" },
    { duration::DUR_WHOLE, "Whol" },
    { duration::DUR_HALF,  "Half" },
    { duration::DUR_QUARTER, "Qrtr" },
    { duration::DUR_8TH, "8th." },
    { duration::DUR_16TH, "16th" },
    { duration::DUR_32ND, "32nd" },
    { duration::DUR_64TH, "64th" },
    { duration::DUR_128TH, "128." },
    { duration::DUR_256TH, "256." },
    { duration::DUR_512TH, "512." },
    { duration::DUR_1024TH, "1024" }
  };
  if (DUR_NAMES.contains(d))
  {
    return os << DUR_NAMES.at(d);
  }
  return os << "Unexpected duration enum " << static_cast<int>(d);
}
}

