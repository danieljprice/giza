Changes in v2.0.0
-----------------
New features:
- added giza_streamplot for streamline plotting from 2D vector fields (#118, #119, #120)
- added native macOS /osx interactive device with rubber-band cursor support (#23, #109)
- implemented fallback glyphs for important Unicode symbols (#101)

Axis, tick, and box drawing:
- fixed tick directions and frame borders on mirrored/inverted world windows (#108, #121; thanks to Karl Glazebrook)
- fixed symmetric internal-axis (A) ticks on giza_box and giza_axis

PGPLOT compatibility:
- fixed vector hardcopy background/foreground colours for PDF, PS, EPS, and SVG (#110)
- fixed /null device colour handling in cpggray (#111)
- use PGPLOT default colour palette when opening devices through the PGPLOT API (#7)
- honour foreground/background order for negative pggray images (#111)
- fixed character height scaling in the PGPLOT interface (#112)
- added perl-PGPLOT unit tests

Interactive devices and input:
- fixed X11 window resize and paper-size reporting after resizing, handle window close button correctly (#117)
- fixed macOS window size, margins, and key-press coordinate reporting
- band/cursor improvements: live crosshair on /xw, band colour, elastic anchor, committed polyline segments during cursor entry (#22, #109)
- fixed filled histogram style (#6)

Build and packaging:
- updated autoconf/automake files (#114)
- build fixes for cairo without X support
- licensing clarifications in source headers (#76)
