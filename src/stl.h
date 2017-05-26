#ifndef __STL_H__
#define __STL_H__

#include "datastructures.h"

class STLReader {
  private:
    std::vector<Vertex>   vertices;
    std::vector<Triangle> triangles;

  public:
    // load an ASCII .stl file
    // fill the vertices and triangle list. the vertices are unified while loading.
    //void loadStl(const char* filename, std::vector<Vertex>& vertices, std::vector<Triangle>& triangles);
    void loadStl(const char* filename);
};

#endif //__STL_H__
