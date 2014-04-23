//
//  Parser.h
//  ray-tracer
//
//  Created by Bryan Bell on 4/2/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Parser__
#define __ray_tracer__Parser__

#include <iostream>
#include <stack>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"

#include "PointLight.h"

#include "Camera.h"

class Parser
{
public:
   /**
    * \brief Constructs the Parser for use with a given scene. Initializes all class variables, and traverses
    *        the given scene's tree at time 0.0f.
    *
    * \param scene [in] The scene to be parsed.
    * \return A constructed Parser containing information concerning the given scene.
    */
   Parser(const aiScene *scene);
   
   /**
    * \brief Traverses the given scene's tree at the given time. Reevaluating all meshes, lights, and cameras.
    *
    * \param time [in] The time at which to use when calculating animation transformations.
    */
   void UpdateScene(float time);
   
   /**
    * \brief Passes back a vector containing all the meshes for the scene.
    *
    * \return A vector containing all the meshes for the scene
    */
   vector<Intersectable<Renderable> *> *Meshes() { return (vector<Intersectable<Renderable> *> *)meshes; };
   
   /**
    * \brief Passes back a vector containing all the lights for the scene.
    *
    * \return A vector containing all the lights for the scene
    */
   vector<AbstractLight *> *Lights() { return lights; };
   
   /**
    * \brief Passes back the camera for the given scene
    *
    * \return The camera for the given scene.
    */
   Camera *Camera() { return camera; };
   
private:
   /**
    * \brief Traverses the given node and its children, adding any meshes they contain to the meshes vector, adding any lights they
    *        contain to the lights vector, and setting the camera if they contain it.
    *
    * \param node [in] The current node in the scene hierarchy.
    */
   void NavigateNode(aiNode *node);
   
   /**
    * \brief Parses the given instance of aiMesh and returns a Mesh object which can be used by ray-tracer
    *
    * \param mesh [in] The aiMesh to be parsed for the creation of a Mesh object
    * \return A Mesh with the contents of the given aiMesh which can be used by ray-tracer
    */
   Mesh *ParseMesh(aiMesh *mesh);
   
   /**
    * \brief Parses the given instance of aiMaterial and returns a material struct which can be used by ray-tracer
    *
    * \param material [in] The aiMaterial to be parsed for the creation of a material struct
    * \return A material with the contents of the given aiMaterial which can be used by ray-tracer
    */
   material ParseMaterial(aiMaterial *material);
   
   /**
    * \brief Parses the given array of aiBones and applies them to the given 
    *        array of vertices
    *
    * \param verts [in/out] The vertices being modified by the bone structures
    * \param bones [in] The bones used to modify the given array of vertices
    * \param numBones [in] The number of bones in the mesh
    */
   void ParseBones(aiVector3D *verts, aiBone** bones, unsigned int numBones);
   
   /**
    * \brief Translates the node given the key frames stored in the given node, and the given time.
    *
    * \param node [in] The node animation information.
    * \return A matrix describing the transformation given by applying the current time to the animation information.
    */
   mat4 TranslateNode(aiNodeAnim *node);
   
   /**
    * \brief Scales the node given the key frames stored in the given node, and the given time.
    *
    * \param node [in] The node animation information.
    * \return A matrix describing the transformation given by applying the current time to the animation information.
    */
   mat4 ScaleNode(aiNodeAnim *node);
   
   /**
    * \brief Rotates the node given the key frames stored in the given node, and the given time.
    *
    * \param node [in] The node animation information.
    * \return A matrix describing the transformation given by applying the current time to the animation information.
    */
   mat4 RotateNode(aiNodeAnim *node);
   
   /**
    * \brief Multiplies the transformations in the transformStack together to get the total transformation at the current node
    *
    * \return A transformation describing the total transformation up to the current node.
    */
   mat4 Transform();
   
   
   const aiScene *scene; // The store for all the scene information
   float time; // The time to use in determining any animation transforms
   
   vector<mat4> *transformStack; // The stack of transforms determined by traversing the scene hierarchy
   
   vector<Mesh *> *meshes; // The meshes parsed from the scene
   vector<AbstractLight *> *lights; // The lights parsed from the scene
   class Camera *camera; // The camera found in the scene
};

#endif /* defined(__ray_tracer__Parser__) */
