//
//  Scene.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Scene__
#define __ray_tracer__Scene__

#include <iostream>
#include <vector>

#include "material.h"

#include "AbstractLight.h"
#include "Intersectable.h"
#include "Renderable.h"
#include "Camera.h"

using namespace std;

class Scene
{
public:
   /**
    * \brief Creates the scene to be traced with the given objects, lights, and camera
    *
    * \param objects [in] The objects in the scene to be traced
    * \param lights [in] The lights in the scene
    * \param camera [in] The camera from which the scene will be traced
    * \return The single instance of the scene with the given objects, or the previously 
    *         setup scene if already initialized
    */
   static Scene *CreateScene(const vector<Intersectable<Renderable> *> *objects,
                             const vector<AbstractLight *> *lights,
                             const Camera *camera);
   
   /**
    * \brief Gets the single instance of the scene running in this program
    *
    * \return The single instance of the scene running in this program
    */
   static Scene *GetScene();
   
   /**
    * \brief Traces the scene
    *
    * \return A two dimensional array of color structs describing the image created
    */
   color **TraceScene();
   
   /**
    * \brief Gets the background color for the scene
    *
    * \return The background color for the scene
    */
   color BackgroundColor() { return background; };
   
   /**
    * \brief Sets the background color for the scene
    *
    * \param bg [in] The color to be used as the background color
    */
   void SetBackgroundColor(color bg) { background = bg; };
   
   /**
    * \brief Gets the number of indirect lighting bounces allowed when tracing the scene
    *
    * \return The number of indirect lighting bounces allowed when tracing the scene
    */
   int IndirectLightingBounces() { return indirectLightingBounces; };
   
   /**
    * \brief Sets the number of indirect lighting bounces allowed when tracing the scene
    *
    * \param bg [in] The the number to be used as the limit of indirect lighting bounces allowed when tracing the scene
    */
   void SetIndirectLightingBounces(int bounces) { indirectLightingBounces = bounces; };
   
   /**
    * \brief Gets the number of indirect lighting rays cast when monte-carlo ray tracing
    *
    * \return The number of indirect lighting rays cast when monte-carlo ray tracing
    */
   int IndirectLightingRays() { return indirectRays; };
   
   /**
    * \brief Sets the number of indirect lighting rays cast when monte-carlo ray tracing
    *
    * \param bg [in] The number of indirect lighting rays to be used when monte-carlo ray tracing
    */
   void SetIndirectLightingRays(int rays) { indirectRays = rays; };
   
   /**
    * \brief Gets the meshes being traced by the scene
    *
    * \return The meshes being traced by the scene
    */
   const vector<Intersectable<Renderable> *> *Objects() { return objects; };
   
private:
   /**
    * \brief Private constructor to maintain the singleton nature of the Scene
    *
    * \param objects [in] The meshes rendered in the scene
    * \param lights [in] The lights in the scene
    * \param camera [in] The camera which the scene is traced from
    */
   Scene(const vector<Intersectable<Renderable> *> *objects,
         const vector<AbstractLight *> *lights,
         const Camera *camera);
   
   /**
    * \brief Traces a single ray through the scene, and returns the color 
    *        gathered by that ray
    *
    * \param cast [in] The ray which is traced through the scene
    * \param depth [in] The number of interactions with the scene the ray has
    *        already had
    * \return The color gathered by the ray
    */
   color TraceRay(ray cast, int depth);
   
   /**
    * \brief Calculates the color of the contacted object at the given contact
    *        point
    *
    * \param cast [in] The ray which hit the given object
    * \param contact [in] The point at which the given object was contacted
    * \param object [in] The object to be used to calculate the color
    * \param depth [in] The number of interactions with the scene the ray
    *        already had
    * \return The color calculated at the given point on the given object
    */
   color CalculateColor(ray cast, vec3 contact, Renderable *object, int depth);
   
   /**
    * \brief Casts rays used for monte-carlo ray tracing into the scene from the
    *        given contact point
    * \param contact [in] The point from which the rays will be cast
    * \param normal [in] The normal around which the indirect rays will be cast
    * \param depth [in] The number of interactions with the scene the ray
    *        already had
    * \return The average color gathered from the indirect rays
    */
   color CastIndirectRays(vec3 contact, vec3 normal, int depth);
   
   /**
    * \brief Determines whether the given point is in shadow, and how dark the
    *        the shadowing is at that point
    *
    * \param contact [in] The point which may be in shadow
    * \param light [in] The light which may be obscured from the point
    * \return A percentage, represented as a float, of how much of the light is
    *         obscured
    */
   float IsPointShadowed(vec3 contact, AbstractLight *light);
   
   static Scene *instance; // The single instance of the scene
   
   const vector<AbstractLight *> *lights; // The lights in the given scene
   const vector<Intersectable<Renderable> *> *objects; // The objects in the given scene
   const Camera *camera; // The camera which the scene is rendered from
   
   color background; // The color used when no objects are intersected
   
   int indirectLightingBounces; // The number of indirect lighting bounces allowed
   int indirectRays; // The number of indirect rays cast when monte-carlo ray-tracing is calculated
};

#endif /* defined(__ray_tracer__Scene__) */
