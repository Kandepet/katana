
/*
 * katana, an experimental stl slicer written in C++0x
 *
 * Usage: katana <input.stl> <output.gcode>
 *
 * This program loads a given .stl (stereolithography data, actually triangle data) file
 * and generates a .gcode (RepRap machine instructions) file that can be printed on a RepRap
 * machine.
 *
 * This process is mostly referred as "slicing", as the printers make objects layer by layer,
 * so the 3d model has to be virtually sliced into thin layers. This however is only the first
 * step, after which perimeters and a crosshatch filling pattern are computed for every layer.
 *
 * The .stl file should describe a clean mesh that is a 2-manifold. That means:
 * * every triangle should touch exactly three triangles along its three edges
 * * on every edge, two triangles share exactly two vertices
 * * every triangle has some positive surface area
 *  a triangle may touch further triangles at its vertices.
 *
 *  currently, many checks are disabled to accept objects breaking any of this rules.
 *  the results however are undefined, albeit sometimes usable.
 *
 * in contrast to other slicers, this one does not need:
 * * triangles don't have to carry valid normals
 * * triangle vertices don't need a defined clockwise or counterclockwise order
 *
 * Based on Schlizzer written by Paul Geisler in 2012.
 * https://github.com/dronus/Schlizzer
 */

#include <stdio.h>
#include <assert.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <array>
#include <math.h>
#include <exception>
#include <fstream>

#include "datastructures.h"
#include "config.h"
#include "stl.h"
#include "gcode.h"
#include "slicer.h"
#include "katana.h"

int main(int argc, const char** argv)
{
  if(argc!=3) {
    printf("Usage: %s <.stl file> <.gcode file>\n",argv[0]);
    return 1;
  }

  Katana::Instance().config.loadConfig("config.ini");

  // load the .stl file
  Katana::Instance().stl.loadStl(argv[1]);

  // create layers and assign touched triangles to them
  Katana::Instance().slicer.buildLayers();

  // create printable segments for every layer
  Katana::Instance().slicer.buildSegments();

  // save filled layers in Gcode format
  Katana::Instance().gcode.write(argv[2]);

  return 0;
}



