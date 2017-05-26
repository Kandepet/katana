Katana
=========

An experimental stl slicer and gcode generator written in C++. This code is based on schlizzer, written by Paul Geisler.

Usage:
  ./katana inputfile.stl outputfile.gcode


Important features missing in respect to Slic3r:

- No discrimination of  'solid' and 'fill' areas. They are handled equally with fill_density=1.
- Only 'rectilinear' fill pattern supported.
- No contour correction of perimeter and infill, so the object exceeds the specified .stl
- Bad route planning leading to bad movement order with useless many and long travels
- Even worse planning for 'non manifold' objects (most of thingyverse i guess..)
- No brim, skirt, cooling etc.
- No automatic placement and z leveling
- Generated Gcode need to be extended before printing
