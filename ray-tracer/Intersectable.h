//
//  Intersectable.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_Intersectable_h
#define ray_tracer_Intersectable_h

#include "ray.h"
#include "boundingbox.h"

template<class T>
class Intersectable
{
public:
   /**
    * \brief Tests the given ray for intersection with the Intersectable
    * 
    * \post This function is allowed to changed the state of the object (Hierarchies, for example,
    *       may want to save which object was just hit to make future calculations quicker)
    *
    * \param ray [in] The ray to test for intersection with the Intersectable
    * \return The object intersected along with the time along the line at which it was intersected
    */
   virtual intersect_info<T> Intersect(ray cast) = 0;
   
   /**
    * \brief Tests the given ray for intersection with the Intersectable
    *
    * \post This function is guaranteed to not change the state of the object (Useful when testing
    *       the Intersectable for secondary purposes like shadowing, closeness, etc.)
    *
    * \param ray [in] The ray to test for intersection with the Intersectable
    * \return The object intersected along with the time along the line at which it was intersected
    */
   virtual intersect_info<T> SafeIntersect(ray cast) = 0;
   
   /**
    * \brief Returns the bounds of the Intersectable
    *
    * \return The bounds of the Intersectable
    */
   virtual bounding_box Bounds() = 0;
};

#endif
