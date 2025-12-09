#include <iostream>
#include <sstream>
#include "tinyxml2.h"
#include "xml_parser.h"
#include "xml_strings.h"

using namespace tinyxml2;

namespace juliet_musicxml
{
namespace internal
{
rest copy_note_members_to_rest(const rest& cr, const note& n)
{
  rest r(cr);
  r.m_duration = n.m_duration;
  r.m_duration_type = n.m_duration_type;
  r.m_stave = n.m_stave;
  r.m_voice = n.m_voice;
  r.m_num_dots = n.m_num_dots;
  return r;
}

class ElementHandler 
{
public:
    virtual ~ElementHandler() = default;
    virtual void handleEnter(const XMLElement& element) {} 
    virtual void handleChildEnter(const XMLElement& element) = 0; 
    virtual void handleText(const XMLText& text) = 0; 
    
    // Store the raw data object into the bar ordered list.
    virtual void handleExit(const XMLElement& element, bar& data) = 0;
};

// Helper function
namespace
{
bool get_named_value(
  const std::string& name, const std::string& match,  // if name matches..
  const std::string& text_value, int& value)  // ..set value and ret true.
{
  if (name == match)
  {
    try 
    { 
      value = std::stoi(text_value); 
      return true;
    } 
    catch (...) {} 
  }
  return false;
}

stem::direction stem_lookup(const std::string str)
{
  if (str == xml::UP) return stem::direction::STEM_UP;
  if (str == xml::DOWN) return stem::direction::STEM_DOWN;
  return stem::direction::STEM_NONE;
}
}

// --- CONCRETE HANDLERS ---

class AttributesHandler : public ElementHandler 
{
private:
    std::string m_child_name;

    // Set to non-zero if we parse a divisions element, and we will 
    //  then create a divisions event on exiting this attributes element.
    divisions m_divisions; 

    // Similarly for these other attributes, we will create events if they
    //  are non-null.

    // This maps one-based stave number to xml 'sign and line' clef definition.
    // We convert that into a clef_sign enum later if the map is non-empty.
    std::map<int, clef_and_line> m_clef_and_line_map;
    int m_current_clef = 1; // default stave for clef in above event.

    time_sig_event m_time_sig;
  
    key_sig_event m_key_sig;
    bool m_key_sig_is_set = false; // set to true if key sig is parsed successfully

    stave_event m_staves;

    void HandleClefElement(const XMLElement& element)
    {
        // This could fail - TODO warning?
        if (element.QueryAttribute(xml::NUMBER, &m_current_clef) != XML_SUCCESS)
        {
            m_current_clef = 1;
        }
    }

    // Convert olde-timee XML clef sign and line map into a clef_event
    auto make_clef_event()
    {
      clef_event ce;
      for (const auto& [stave_1_based, clef_info] : m_clef_and_line_map)
      {
        ce.m_clef_map[stave_1_based - 1] = get_clef_sign(clef_info);
      }
      return std::make_unique<clef_event>(ce);
    }

public:  
    void handleEnter(const XMLElement& element) override 
    { 
        m_divisions.m_num_divisions = 0;
        m_time_sig.m_fraction = fraction(0, 0); // 0/0 means not set
        m_staves.m_stave_info.m_num_staves = 0;
    }

    void handleChildEnter(const XMLElement& element) override 
    { 
        m_child_name = element.Name(); 

        if (m_child_name == xml::CLEF)
        {
            HandleClefElement(element);
        }
    }

    void handleText(const XMLText& text) override 
    {
        const std::string textValue = text.Value();
        if (get_named_value(m_child_name, xml::BEATS, textValue, m_time_sig.m_fraction.num)) {}
        else if (get_named_value(m_child_name, xml::BEAT_TYPE, textValue, m_time_sig.m_fraction.denom)) {}
        else if (get_named_value(m_child_name, xml::STAVES, textValue, m_staves.m_stave_info.m_num_staves)) {}
        else if (get_named_value(m_child_name, xml::DIVISIONS, textValue, m_divisions.m_num_divisions)) {}
        else if (m_child_name == xml::SIGN) 
        { 
            m_clef_and_line_map[m_current_clef].m_sign = textValue; 
        }
        else if (m_child_name == xml::LINE)
        {
            int line_num = 0;
            if (get_named_value(m_child_name, xml::LINE, textValue, line_num))
            {
                // Will create entry in map if it doesn't exist
                m_clef_and_line_map[m_current_clef].m_line = line_num;
            }
        }
        else if (m_child_name == xml::CLEF_OCTAVE_CHANGE)
        {
            int octave_change = 0;
            if (get_named_value(m_child_name, xml::CLEF_OCTAVE_CHANGE, textValue, octave_change))
            {
                // Will create entry in map if it doesn't exist
                m_clef_and_line_map[m_current_clef].m_octave_change = octave_change;
            }
        }
        else if (m_child_name == xml::FIFTHS)
        { 
            int num_fifths = 0;
            if (get_named_value(m_child_name, xml::FIFTHS, textValue, num_fifths))
            {
               m_key_sig_is_set = m_key_sig.set_from_num_fifths(num_fifths);
            }  
        }
    }

    void handleExit(const XMLElement& element, bar& data) override 
    {
        // This order is partly so the renderer gets the info in the 
        //  order it needs (we need to know about staves first). 
        // Also for how the music looks, (we want key sig before time sig).

        if (m_staves.m_stave_info.m_num_staves != 0)
        {
          data.m_events.emplace_back(std::make_unique<stave_event>(m_staves));
        }

        if (m_divisions.m_num_divisions != 0)
        { 
          data.m_events.emplace_back(std::make_unique<divisions>(m_divisions));
        }

        if (!m_clef_and_line_map.empty())
        {
          // We have some clef info - convert to clef_sign enums and
          //  create clef event.
          data.m_events.emplace_back(make_clef_event());
        }

        if (m_key_sig_is_set)
        {
          data.m_events.emplace_back(std::make_unique<key_sig_event>(m_key_sig));
        }

        if (m_time_sig.m_fraction.denom != 0)
        {
          data.m_events.emplace_back(std::make_unique<time_sig_event>(m_time_sig));
        }
    }    
};

class NoteHandler : public ElementHandler {
private:
    bool m_is_rest = false; // in MusicXML, a note can be a rest
    note m_note;
    rest m_rest; 
    std::string m_child_name;

    void HandleRestElement(const XMLElement& element)
    {
        m_is_rest = true;

        // <rest> element can have "measure" attribute, for whole-bar rest.
        const char* measure_text = element.Attribute(xml::MEASURE);
        m_rest.m_is_whole_bar = (measure_text && std::string(measure_text) == xml::YES);
    }

public:
    void handleEnter(const XMLElement& element) override 
    {
        // Check for chord status
        if (element.FirstChildElement(xml::CHORD)) { m_note.m_is_chord = true; }
    }

    void handleChildEnter(const XMLElement& element) override
    {
        const std::string name = element.Name();

        // Skip the empty <chord/> 
        if (name != xml::CHORD) 
        { 
            m_child_name = name; 
        }

        // ..but do check for rest "measure" attrib
        if (name == xml::REST)
        {
            HandleRestElement(element); 
        }
    }

    void handleText(const XMLText& text) override {
        const std::string textValue = text.Value();
        if (m_child_name == xml::STEP) { m_note.m_pitch.m_step = textValue.empty() ? '\0' : textValue[0]; }
        else if (get_named_value(m_child_name, "octave", textValue, m_note.m_pitch.m_octave)) {}
        else if (get_named_value(m_child_name, "alter", textValue, m_note.m_pitch.m_alter)) {}
        else if (get_named_value(m_child_name, "duration", textValue, m_note.m_duration)) {}
        else if (get_named_value(m_child_name, xml::STAFF, textValue, m_note.m_stave)) 
        {
          m_note.m_stave--; // Convert one-based xml value to zero-based index.
        }
        else if (m_child_name == "type") { m_note.m_duration_type = from_string(textValue).value(); }
        else if (m_child_name == "stem") { m_note.m_stem.m_direction = stem_lookup(textValue); }
        else if (get_named_value(m_child_name, "voice", textValue, m_note.m_voice)) {}
    }

    void handleExit(const XMLElement& element, bar& data) override 
    {
        if (m_is_rest)
        {
            data.m_events.emplace_back(std::make_unique<rest>(copy_note_members_to_rest(m_rest, m_note)));
        }
        else
        {
            auto exp = m_note.m_pitch.calc_midi_pitch();
            if (exp)
            {
              m_note.m_pitch.m_midi_pitch = exp.value();
            } // else log error?
            data.m_events.emplace_back(std::make_unique<note>(m_note));
        }
    }
};

class BackupHandler : public ElementHandler {
private:
    backup m_backup;
    std::string m_child_name;
public:
    void handleChildEnter(const XMLElement& element) override 
    { 
      m_child_name = element.Name(); 
    }

    void handleText(const XMLText& text) override 
    {
        get_named_value(m_child_name, "duration", text.Value(), m_backup.m_duration);
    }

    void handleExit(const XMLElement& element, bar& data) override {
        data.m_events.emplace_back(std::make_unique<backup>(m_backup));
    }
};

class ForwardHandler : public ElementHandler {
private:
    forward m_forward;
    std::string m_child_name;
public:
    void handleChildEnter(const XMLElement& element) override 
    { 
        m_child_name = element.Name(); 
    }

    void handleText(const XMLText& text) override 
    {
        get_named_value(m_child_name, "duration", text.Value(), m_forward.m_duration);
    }

    void handleExit(const XMLElement& element, bar& data) override 
    {
        data.m_events.emplace_back(std::make_unique<forward>(m_forward));
    }
};

// Visitor, implements tinyxml2 XMLVisitor
class MusicDataVisitor : public XMLVisitor {
public:
    bar measureData;

private:
    std::unique_ptr<ElementHandler> activeHandler;
    std::string currentTopLevelElement;

    std::unique_ptr<ElementHandler> createHandler(const std::string& elementName) 
    {
        if (elementName == "note") { return std::make_unique<NoteHandler>(); }
        else if (elementName == "attributes") { return std::make_unique<AttributesHandler>(); }
        else if (elementName == "backup") { return std::make_unique<BackupHandler>(); }
        else if (elementName == "forward") { return std::make_unique<ForwardHandler>(); }
        return nullptr;
    }

public:
    bool VisitEnter(const XMLElement& element, const XMLAttribute* firstAttribute) override 
    {
        const std::string name = element.Name();

        if (activeHandler) {
            activeHandler->handleChildEnter(element);
        } else {
            activeHandler = createHandler(name);

            if (activeHandler) {
                activeHandler->handleEnter(element);
                currentTopLevelElement = name;
            }
            else
            {
//#define WARN
#ifdef WARN
std::cout << "Warning! No handler for this element: \"" 
    << name 
    << "\", line: "
    << element.GetLineNum() 
    << std::endl;
#endif // WARN
            }
        }
        return true;
   }

    bool Visit(const XMLText& text) override {
        if (activeHandler) {
            activeHandler->handleText(text);
        }
        return true;
    }

    bool VisitExit(const XMLElement& element) override {
        const std::string name = element.Name();

        if (activeHandler && name == currentTopLevelElement) {
            activeHandler->handleExit(element, measureData);
            activeHandler.reset();
            currentTopLevelElement.clear();
        }

        return true;
    }
};

// Parses the XML content of a single bar using the Visitor pattern.
bar parse_bar(XMLNode* measureNode) 
{
    MusicDataVisitor visitor;
    if (measureNode) {
        measureNode->Accept(&visitor);
    }
    return std::move(visitor.measureData);
}   


expected_score parse_xml_doc(XMLDocument& doc)
{
    XMLElement* root = doc.RootElement();
    if (root == nullptr)
    {
      return tl::unexpected("XML Document has no root node!");
    }

    score scoreData;
            
    // Iterate through all <part> elements
    for (XMLElement* partElem = root->FirstChildElement("part");
         partElem != nullptr;
         partElem = partElem->NextSiblingElement("part"))
    {
        const char* partId = partElem->Attribute("id");
        if (!partId) continue;
        
        int part_index = scoreData.add_new_part(partId);

        // Iterate over all bars in the part.
        int bar_number = 1;
        for (XMLNode* measureNode = partElem->FirstChildElement("measure");
             measureNode != nullptr;
             measureNode = measureNode->NextSiblingElement("measure"))
        {
            std::unique_ptr<bar> b = std::make_unique<bar>(parse_bar(measureNode));

            // TODO We want each event to know which part it's in.
            // So should each event point back to its owning bar? Or part?
            b->set_part_index(part_index); 

            b->m_bar_number = bar_number++;
            scoreData.add_bar_for_part(part_index, b); 
        }
    }
    return std::move(scoreData);
}
} // namespace internal

expected_score xml_parser::parse_string(const std::string& string)
{
    XMLDocument doc;
    XMLError result = doc.Parse(string.c_str());
        
    if (result != XML_SUCCESS) 
    {
        std::stringstream ss;
        ss << "XML Parse Error: " << doc.ErrorIDToName(result);
        return tl::unexpected(ss.str());
    }   

    return internal::parse_xml_doc(doc);
}

expected_score xml_parser::parse_file(const std::string& filename)
{
#ifdef WARN
std::cout << "Parsing file " << filename << std::endl;
#endif

    XMLDocument doc;
    XMLError result = doc.LoadFile(filename.c_str());
        
    if (result != XML_SUCCESS) 
    {
        std::stringstream ss;
        ss << "XML Parse Error: " << doc.ErrorIDToName(result);
        return tl::unexpected(ss.str());
    }   

    return internal::parse_xml_doc(doc);
}
}

