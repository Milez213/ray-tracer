//
//  Octree.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Octree.h"

Octree::Octree(vector<Intersectable<Renderable> *> *objects)
{
   vec3 min = vec3(INF);
   vec3 max = vec3(-INF);
   vec3 oMin;
   vec3 oMax;
   
   int numObjects = (int)objects->size();
   for (int i = 0; i < numObjects; i++)
   {
      oMin = objects->at(i)->Bounds().minimum;
      oMax = objects->at(i)->Bounds().maximum;
      if (oMin.x < min.x) { min.x = oMin.x; }
      if (oMax.x > max.x) { max.x = oMax.x; }
      if (oMin.y < min.y) { min.y = oMin.y; }
      if (oMax.y > max.y) { max.y = oMax.y; }
      if (oMin.z < min.z) { min.z = oMin.z; }
      if (oMax.z > max.z) { max.z = oMax.z; }
   }
   
   root = octree_node(bounding_box(min, max));
   
   for (int i = 0; i < numObjects; i++)
   {
      root.add(objects->at(i));
   }
}

intersect_info<Renderable> Octree::Intersect(ray cast)
{
   intersect_info<Renderable> info = root.intersect(cast);
   
   return info;
}

intersect_info<Renderable> Octree::SafeIntersect(ray cast)
{
   intersect_info<Renderable> info = root.intersect(cast);
   
   return info;
}
