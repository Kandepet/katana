
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
#include "katana.h"
#include "stl.h"
#include "gcode.h"

// save Gcode
// iterates over the previously generated layers and emit gcode for every segment
// uses some configuration values to decide when to retract the filament, how much
// to extrude and so on.
//void GCode::write(const char* filename, std::vector<Layer>& layers, float min_z)
void GCodeWriter::write(const char* filename)
{

  printf("Saving Gcode...\n");
  FILE* file=fopen(filename,"w");

  fprintf(file,"%s\n",Katana::Instance().config.getString("start_gcode"));

  // segments shorter than this are ignored
  float skipDistance=.01;

  // compute extrusion factor, that is the amount of filament feed over extrusion length
  float dia=Katana::Instance().config.get("filament_diameter");
  float filamentArea=3.14159f*dia*dia/4;
  float extrusionVolume=Katana::Instance().config.get("nozzle_diameter")*Katana::Instance().config.get("layer_height");
  float extrusionFactor=extrusionVolume/filamentArea*Katana::Instance().config.get("extrusion_multiplier");

  // retract filament if traveling
  float retract_length=Katana::Instance().config.get("retract_length");
  float retract_before_travel=Katana::Instance().config.get("retract_before_travel");

  // statistical values shown to the user
  int travels=0, longTravels=0, extrusions=0;
  int travelsSkipped=0, extrusionsSkipped=0;
  float travelled=0, extruded=0;

  // offset of the emitted Gcode coordinates to the .stl ones
  //Vertex offset={75,75,Katana::Instance().config.get("z_offset")-Katana::Instance().min_z};
  Vertex offset={0,0,0};

  Vertex position={0,0,0};
  for(unsigned int i=0; i<Katana::Instance().layers.size(); i++){
    Layer& l=Katana::Instance().layers[i];
    fprintf(file, "G92 E0\n");                        // reset extrusion axis

    float feedrate=(i==0) ? 500.f : 1800.f ;
    fprintf(file, "G1 Z%f F%f ;layer %d\n",l.z+offset.z,feedrate,i); // move to layer's z plane

    float extrusion=(i==0) ? 1 : 0; // extrusion axis position

    for(unsigned int j=0; j<l.segments.size(); j++){
      Vertex& v0=l.segments[j].vertices[0];
      Vertex& v1=l.segments[j].vertices[1];
      //assert(v0.z==l.z);
      //assert(v1.z==l.z);
      // skip segment with NaN or Infinity caused by numeric instablities
      if(v0.z!=l.z) continue;
      if(v1.z!=l.z) continue;


      // reorder segment for shorter or zero traveling
      //if(distance(v1,position)<distance(v0,position))
      if(v1.distance(position)<v0.distance(position))
        std::swap(v0,v1);

      // check distance to decide if we need to travel
      float d=v0.distance(position);
      if(d>skipDistance){
        fprintf(file, "; segments not connected\n");
        // the sements are not connected, so travel without extrusion
        if(d>retract_before_travel){
          // we travel some time, do retraction
          extrusion-=retract_length;
          fprintf(file,"G1 F1800.0 E%f ; Retracting filament\n",extrusion);
          //G92 E0
        }
        // emit G1 travel command
        fprintf(file,"G1 X%f Y%f ; Traveling without extrusion\n",v0.x+offset.x,v0.y+offset.y);
        if(d>retract_before_travel){
          // we travelled some time, undo retraction
          extrusion+=retract_length;
          fprintf(file,"G1 F1800.0 E%f ; Undoing retraction\n",extrusion);
          longTravels++;
        }
        travels++;
        travelled+=v0.distance(position);
        position=v0;
      }else   // the segments where connected or not far away
        travelsSkipped++;

      extrusion+=extrusionFactor*v0.distance(v1); // compute extrusion by segment length
      if(v1.distance(position)>skipDistance){
        // emit G1 extrusion command
        fprintf(file,"G1 X%f Y%f E%f\n",v1.x+offset.x,v1.y+offset.y,extrusion);
        extrusions++;
        extruded+=v1.distance(position);
        position=v1;
      }else   // the segment is to short to do extrusion
        extrusionsSkipped++;
    }
  }

  fprintf(file, "%s",Katana::Instance().config.getString("end_gcode"));

  // print some statisitcs
  printf("Saving complete. %ld bytes written. %d travels %.0f mm, %d long travels, %d extrusions %.0f mm, %d travel skips, %d extrusion skips\n",
      ftell(file),travels, travelled, longTravels, extrusions, extruded, travelsSkipped, extrusionsSkipped);
}
