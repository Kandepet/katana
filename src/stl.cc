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

// load an ASCII .stl file
// fill the vertices and triangle list. the vertices are unified while loading.
//void STLReader::loadStl(const char* filename, std::vector<Vertex>& vertices, std::vector<Triangle>& triangles)
void STLReader::loadStl(const char* filename)
{
  // as .stl stores unconnected triangles, any vertex found is usually repeated in
  // several more triangles. to remesh that heap of triangles, we unify those vertices.

  // collection of unique vertices and their index number
  std::map<Vertex,int> uniqueVertices;

  // collection of vertex indicies to reconstruct triangles after vertex merging
  std::vector<int> indices;
  // collection of triangle normals
  std::vector<Vertex> normals;

  printf("Loading %s...\n",filename);
  FILE* file=fopen(filename,"r");
  char line[256];

  while(!feof(file)){
    // read file line by line
    if(fgets(line, sizeof(line), file)){
      Vertex p,n;
      // we scan for vertex definitions, their triangle linking is given by
      // groups of three consecutive definitions.
      DEBUG("Scanning: " << line);

      // scan for triangle normal definition
      int found=sscanf(line," facet normal %e %e %e",&n.x,&n.y,&n.z);
      if(found)
        normals.push_back(n);     // store triangle normal

      // scan for vertex definition
      found=sscanf(line," vertex %e %e %e",&p.x,&p.y,&p.z);
      if(found) {
        // we found a vertex declaration
        // check if this vertex is already known
        int index;
        if(uniqueVertices.count(p)==1) {
          // we know the vertex, so get its index
          index=uniqueVertices[p];
          DEBUG("Found duplicate(" << index << ") vertex: " << p.x << ", " << p.y << ", " << p.z);
        }else{
          // this is a new vertex, so store it
          Katana::Instance().vertices.push_back(p);
          index=Katana::Instance().vertices.size()-1; // the new vertex is the last element
          uniqueVertices[p]=index; // store index
          DEBUG("Found " << index << " vertex: " << p.x << ", " << p.y << ", " << p.z);
        }
        indices.push_back(index); // store index in triangle order
      }
    }
  }
  fclose(file);

  DEBUG("Indices: " << indices.size() << " normals: " << normals.size());

  // if we read triangles only, there are triangles*3 vertex indices
  assert(indices.size()%3 == 0);
  assert(indices.size()==normals.size()*3);

  // create triangles
  for(unsigned int i=0; i<indices.size(); i+=3)
  {
    Triangle t;
    // store vertex pointers
    for(int j=0; j<3; j++)
      t.vertices[j]=&Katana::Instance().vertices[indices[i+j]];
    // sort vertices bottom-up for later operations
    t.sortTriangleVertices();
    // store normal
    t.normal=normals[i/3];
    // add triangle
    Katana::Instance().triangles.push_back(t);
  }

  printf("Loading complete: %u vertices read, %u unique, %u triangles\n",(int)indices.size(),(int)Katana::Instance().vertices.size(),(int)Katana::Instance().triangles.size());
}

