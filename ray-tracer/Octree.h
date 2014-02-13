//
//  Octree.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Octree__
#define __ray_tracer__Octree__

#include <iostream>
#include <vector>

#include "glm/glm.hpp"

#include "boundingbox.h"
#include "ray.h"

#include "Intersectable.h"
#include "Renderable.h"

using namespace std;
using namespace glm;

#define NUM_CHILDREN 8
#define MAX_OBJECTS 3

struct octree_node
{
   octree_node *children;
   Intersectable<Renderable> **objects;
   
   short numObjects;
   
   bounding_box bounds;
   
   inline octree_node()
   {
      children = NULL;
      objects = new Intersectable<Renderable>*[MAX_OBJECTS];
      
      numObjects = 0;
      
      bounds = bounding_box();
   }
   
   inline octree_node(bounding_box box)
   {
      children = NULL;
      objects = new Intersectable<Renderable>*[MAX_OBJECTS];
    
      numObjects = 0;
    
      bounds = box;
   }
   
   /*inline ~octree_node()
   {
      if (numObjects)
      {
         delete[] objects;
      }
      else
      {
         for (int i = 0; i < NUM_CHILDREN; i++)
         {
            children[i].~octree_node();
         }
    
         delete[] children;
      }
   }*/
    
   void add(Intersectable<Renderable> *object)
   {
      if (numObjects < MAX_OBJECTS)
      {
         objects[numObjects++] = object;
      }
      else if (children == NULL)
      {
         children = (octree_node *)malloc(sizeof(octree_node) * NUM_CHILDREN);
         
         vec3 octant = (bounds.maximum - bounds.minimum) / 2.0f;
         vec3 oMin;
         vec3 oMax;
         
         int index;
         
         for (int x = 0; x < 2; x++)
         {
            for (int y = 0; y < 2; y++)
            {
               for (int z = 0; z < 2; z++)
               {
                  oMin = vec3(bounds.minimum + octant * vec3(x, y, z));
                  oMax = oMin + octant;
                  
                  index = x * 4 + y * 2 + z;
                  children[index] = octree_node(bounding_box(vec3(oMin), vec3(oMax)));
                  
                  for (int i = 0; i < MAX_OBJECTS; i++)
                  {
                     if (children[index].bounds.intersects(objects[i]->Bounds()))
                     {
                        children[index].add(objects[i]);
                     }
                  }
                  
                  if (children[index].bounds.intersects(object->Bounds()))
                  {
                     children[index].add(object);
                  }
               }
            }
         }
         
         numObjects = 0;
         delete[] objects;
      }
      else
      {
         for (int i = 0; i < NUM_CHILDREN; i++)
         {
            if (children[i].bounds.intersects(object->Bounds()))
            {
               children[i].add(object);
            }
         }
      }
   }
   
   void intersect(ray cast, vector<Intersectable<Renderable> *> *collected)
   {
      float tMin, tMax;
      
      float xMin = (bounds.minimum.x - cast.origin.x) / cast.direction.x;
      float xMax = (bounds.maximum.x - cast.origin.x) / cast.direction.x;
      if (xMin > xMax)
      {
         float temp = xMin;
         xMin = xMax;
         xMax = temp;
      }
      tMin = xMin;
      tMax = xMax;
      
      float yMin = (bounds.minimum.y - cast.origin.y) / cast.direction.y;
      float yMax = (bounds.maximum.y - cast.origin.y) / cast.direction.y;
      if (yMin > yMax)
      {
         float temp = yMin;
         yMin = yMax;
         yMax = temp;
      }
      
      if ((tMin > yMax) || (yMin > tMax))
      {
         return;
      }
      
      if (yMin > xMin)
      {
         tMin = yMin;
      }
      if (yMax < xMax)
      {
         tMax = yMax;
      }
      
      float zMin = (bounds.minimum.z - cast.origin.z) / cast.direction.z;
      float zMax = (bounds.maximum.z - cast.origin.z) / cast.direction.z;
      if (zMin > zMax)
      {
         float temp = zMin;
         zMin = zMax;
         zMax = temp;
      }
      
      if ((tMin > zMax) || (zMin > tMax))
      {
         return;
      }
      
      if (zMin > xMin)
      {
         tMin = zMin;
      }
      if (zMax < xMax)
      {
         tMax = zMax;
      }
      
      if (tMax < 0.0f)
         return;
      
      if (numObjects)
      {
         for (int i = 0; i < numObjects; i++)
         {
            collected->push_back(objects[i]);
         }
      }
      else
      {
         for (int i = 0; i < NUM_CHILDREN; i++)
         {
            children[i].intersect(cast, collected);
         }
      }
   }
   
   
};

class Octree : public Intersectable<Renderable>
{
public:
   Octree(vector<Intersectable<Renderable> *> *objects);
   
   virtual intersect_info<Renderable> Intersect(ray cast);
   virtual bounding_box Bounds() { return bounding_box(); };
   
private:
   
   octree_node root;
};

#endif /* defined(__ray_tracer__Octree__) */
