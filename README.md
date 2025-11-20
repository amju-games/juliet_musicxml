# juliet_musicxml
## Single-header library for reading MusicXML

The default output is a sequence of renderable primitives,
 and optionally MIDI note events and animation hints. 

### Third party code included in this library
 - [tinyxml2](https://github.com/leethomason/tinyxml2)
 - [catch2](https://github.com/catchorg/Catch2)
 - [tl::expected](https://github.com/TartanLlama/expected)

### Build and run tests
```
cd tests
make
make run
```

## Brain dump
Steps in the process

Step 1 - Generate raw events
Parse input; if input is XML, we use tinyxml2 (We can parse other formats as long as we end up with a sequence of raw events.)
Visit nodes in xml tree, generating sequence of raw events. The order is simply the order in which we found each event during parsing.
Normalise time values: different parts can have different divisions, and each bar could have different divisions too. Time values should be in the same units across parts and bars.
Expand out attributes so we get distinct events for clefs, key sig, time sig, etc.
Adjust times for events (i.e. track <backup> and <forward> MusicXML elements).
Adjust events, e.g. decide on final pitch for notes. 
Events are not MusicXML-specific at this point.

Step 2 - Group events into "verticals"
Sort events in each bar by start time - all events with the same start time form a "vertical"
First pass over bar verticals to get widths
Split bars into lines using page info
Adjust widths
Split ties, slurs, beams etc ("attachments") that are split by line breaks

Step 3 - Render (default impl)
Output times for all glyphs that follow, for animation, if required
Render out each element in a vertical into glyphs or text
Create curves for ties, slurs: can be split across lines 
Create quads/lines for everything else

Output the music as MIDI note events if required


