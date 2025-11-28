#pragma once

// bleurgh.... I'm trying to turn the outline code in example/main.cpp
//  into a policy-based template class... needs more thought

namespace juliet_musicxml
{
// We make use of expected internally... 
// As intermediate steps can return an unexpected, the unexpected type needs
//  to be the same all the way through. So e.g. if we use an error code, all
//  the codes need to live in the same enum class, or what have you.
// TODO Is this really any better than throwing an exception if anything goes
//  wrong? It could be better if you had exceptions disabled for performance,
//  but I don't know if that's a thing anymore. 
// ...maybe that could be a POLICY!
// ...using tinyxml2 is also a policy, it's our default policy and so the
//  include should only live in the impl file for that policy. Also put an
//  #ifdef around that impl file.
template <class RETURN_TYPE,  // TODO we can use decltype here, right? Or just auto?
  class PARSER = tinyxml2_parser, // Default impl: use tinyxml2
  class INPUT = file,  // we need a way of decoupling parser type from input type. Does this do the job?
   //  -- No just have different parser types, which can specialise the main bulk of the parsing
  class OUTPUT = std::ostream, // Decouple renderer type from where it renders to
  class RENDERER = text_renderer>
class musicxml_renderer : public PARSER, public RENDERER
{
EXPECTED_OUTPUT render_musicxml(INPUT& input)
{
  // The PARSER type needs to be able to accept the input type, which can be a 
  //  file, a string, maybe some kind of interactive thing.
  // So this stage can go wrong in lots of ways - e.g. file access, xml parse 
  //  error, semantic error, etc.

  const auto parse_result = PARSER::parse(input);

  // can we do this, where is_error is defined by a policy?
  if (is_error(parse_result)) // ??
  {
    return parse_result; // ??

    // Or do we just return xml_parse_result.error());  
    //  Either way we are relying on expected, more explicitly this way:
    return tl::unexpected(parse_result.error()); 
  }

  // Perhaps normalizing times belongs in xml parser. Because backup is a 
  //  music xml thing, and other parsers could just create a score
  //  with all times as they should be.
  time_normalizer tn;  
  // should the above type be a policy?
  // No - the internal slice is private impl and not policy-based.
  tn.normalize_times(parse_result);

  verticalizer v;
  v.group_verticals(normalized_times_result);

  // The output renderer is very much a policy.
  return RENDERER::render(verticalized_result);
}

};
}

