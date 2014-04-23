//
//  Parser.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 4/2/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Parser.h"

#define CAM_NAME "camera1"

Parser::Parser(const aiScene *scn)
{
   scene = scn; // Sets the scene to be parsed to a member of the object for ease of access
   time = 0.0f; // Initialize animations to time 0 to start
   
   transformStack = new vector<mat4>(); // Initialize the transform stack
   
   lights = new vector<AbstractLight *>(); // Initialize the lights vector
   meshes = new vector<Mesh *>(); // Initialize the mesh vector
   
   aiNode *root = scn->mRootNode; // Get the root node to start traversing the hierarchy
   
   /* For each child of the root node, gather scene information */
   for (int i = 0; i < root->mNumChildren; i++)
   {
      NavigateNode(root->mChildren[i]);
   }
}

void Parser::UpdateScene(float t)
{
   aiNode *root = scene->mRootNode; // Get the root node so we can start traversing the hierarchy
   time = t; // Set the time for animations to be resolved
   
   lights->clear(); // Empty the lights vector as it's about to be refilled
   meshes->clear(); // Do the same for teh meshes vector
   
   /* For each child of the root node, gather scene information */
   for (int i = 0; i < root->mNumChildren; i++)
   {
      NavigateNode(root->mChildren[i]);
   }
}


#pragma mark Private Functions

void Parser::NavigateNode(aiNode *node)
{
   int numChildren = node->mNumChildren; // The number of children this node has
   mat4 transform = mat4(1.0f); // The transformation at this node
   
   aiAnimation *animation = NULL; // Temporary holder for animation candidate
   aiNodeAnim *nodeAnim = NULL; // The animation for this particular node
   aiNodeAnim *nodeTemp = NULL; // Temporary holder for node animation candidate
   
   /* Decide whether this node is animated or not */
   for (int i = 0; i < scene->mNumAnimations && !nodeAnim; i++)
   {
      animation = scene->mAnimations[i];
      for (int j = 0; j < animation->mNumChannels && !nodeAnim; j++)
      {
         nodeTemp = animation->mChannels[j];
         if (nodeTemp->mNodeName == node->mName)
         {
            nodeAnim = nodeTemp;
         }
      }
   }
   
   /* If this node is animated, calculate the change to the transformation for this node */
   if (nodeAnim)
   {
      transform = transform * TranslateNode(nodeAnim);
      transform = transform * ScaleNode(nodeAnim);
      transform = transform * RotateNode(nodeAnim);
   }
   /* If not, transform the node by the default transformation */
   else
   {
      transform = mat4(node->mTransformation.a1, node->mTransformation.a2,
                       node->mTransformation.a3, node->mTransformation.a4,
                       node->mTransformation.b1, node->mTransformation.b2,
                       node->mTransformation.b3, node->mTransformation.b4,
                       node->mTransformation.c1, node->mTransformation.c2,
                       node->mTransformation.c3, node->mTransformation.c4,
                       node->mTransformation.d1, node->mTransformation.d2,
                       node->mTransformation.d3, node->mTransformation.d4);
   }
   
   transformStack->push_back(transform);
   
   /* If the node contains a mesh, it creates a Mesh object representative of it and adds it to meshes */
   if (node->mNumMeshes > 0)
   {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[0]]; // The mesh of the current node
      Mesh *myMesh = ParseMesh(mesh);
      meshes->push_back(myMesh);
   }
   
   /* Loop through the list of cameras in the scene, if any of them share a name with the current node
    * create the camera to be used by ray-tracer */
   for (int i = 0; i < scene->mNumCameras; i++) {
      aiCamera *cam = scene->mCameras[i]; // The camera currently being inspected
      
      /* If the current camera has the same name as the current node, then a the scene camera is created
       * with the current camera's attributes */
      if (cam->mName == node->mName && cam->mName == aiString(CAM_NAME)) {
         vec3 eye = transform_point(vec3(cam->mPosition.x, cam->mPosition.y, cam->mPosition.z), Transform()); // The position of the camera
         vec3 lookAt = transform_point(vec3(cam->mLookAt.x, cam->mLookAt.y, cam->mLookAt.z), Transform()); // The focus of the camera
         
         camera = new class Camera(eye, lookAt, vec3(cam->mUp.x, cam->mUp.y, cam->mUp.z));
      }
   }
   
   /* Loop through the lights in the scene, if any of them share a name with the current node
    * create a light to be used by ray-tracer with the current attributes and transformation */
   for (int i = 0; i < scene->mNumLights; i++) {
      aiLight *lgt = scene->mLights[i]; // The light currently being inspected
      
      /* If the current light's name and the current node's name match, add the light to the scene */
      if (lgt->mName == node->mName) {
         vec3 pos = transform_point(vec3(lgt->mPosition.x, lgt->mPosition.y, lgt->mPosition.z), Transform());
         color col = color(lgt->mColorDiffuse.r, lgt->mColorDiffuse.g, lgt->mColorDiffuse.b);
         
         PointLight *light = new PointLight(col, vec3(pos.x, pos.y, pos.z)); // The light to be added to the scene
         lights->push_back(light);
      }
   }
   
   /* Now navigate through all the child nodes to collect their meshes */
   for (int i = 0; i < numChildren; i++)
   {
      NavigateNode(node->mChildren[i]);
   }
   
   transformStack->pop_back(); // Pop the current node's transformation off the stack so it doesn't affect other nodes
}

Mesh *Parser::ParseMesh(aiMesh *mesh)
{
   Mesh *parsed; // The mesh parsed from the given aiMesh
   material mat; // The material of the parsed mesh
   
   aiVector3D *verts = mesh->mVertices; // The vertices that define the mesh
   aiVector3D *norms = mesh->mNormals; // The normals for the vertices
   vector<Intersectable<Renderable> *> meshTriangles; // The vector describing the faces of the mesh
   
   if (mesh->HasBones())
   {
      ParseBones(verts, mesh->mBones, mesh->mNumBones);
   }
   
   /* For each face create a triangle and add it to meshTriangles */
   for (int j = 0; j < mesh->mNumFaces; j++) {
      aiFace face = mesh->mFaces[j]; // The current face
      
      /* If the given face has enough vertices to form a triangle, add it */
      if (face.mNumIndices >= 3) {
         aiMaterial *mater = scene->mMaterials[mesh->mMaterialIndex]; // The material associated with the mesh
         mat = ParseMaterial(mater);
         Triangle *tri = new Triangle(vec3(verts[face.mIndices[0]].x,
                                           verts[face.mIndices[0]].y,
                                           verts[face.mIndices[0]].z),
                                      vec3(verts[face.mIndices[1]].x,
                                           verts[face.mIndices[1]].y,
                                           verts[face.mIndices[1]].z),
                                      vec3(verts[face.mIndices[2]].x,
                                           verts[face.mIndices[2]].y,
                                           verts[face.mIndices[2]].z),
                                      vec3(norms[face.mIndices[0]].x,
                                           norms[face.mIndices[0]].y,
                                           norms[face.mIndices[0]].z),
                                      vec3(norms[face.mIndices[1]].x,
                                           norms[face.mIndices[1]].y,
                                           norms[face.mIndices[1]].z),
                                      vec3(norms[face.mIndices[2]].x,
                                           norms[face.mIndices[2]].y,
                                           norms[face.mIndices[2]].z),
                                      mat);
         meshTriangles.push_back(tri);
      }
   }
   parsed = new Mesh(&meshTriangles, mat, Transform()); // Create a mesh from the given faces
   
   return parsed;
}

material Parser::ParseMaterial(aiMaterial *mat)
{
   material parsed = material(); // The material parsed from
   
   int model; // Key representing the shading model which should be used
   aiColor3D diff(0.0f, 0.0f, 0.0f); // The diffuse color of the mesh
   aiColor3D spec(0.0f, 0.0f, 0.0f); // The specular color of the mesh
   aiColor3D ambt(0.0f, 0.0f, 0.0f); // The ambient color of the mesh
   float opac; // The opacity of the mesh
   
   
   /* extract the properties from the aiMaterial and put them into the material struct */
   aiReturn prop = mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
   prop = mat->Get(AI_MATKEY_COLOR_SPECULAR, spec);
   prop = mat->Get(AI_MATKEY_COLOR_AMBIENT, ambt);
   prop = mat->Get(AI_MATKEY_OPACITY, opac);
   prop = mat->Get(AI_MATKEY_SHADING_MODEL, model);
   parsed.diffuse = color(diff.r, diff.g, diff.b);
   parsed.specular = color(spec.r, spec.g, spec.b);
   parsed.ambient = color(ambt.r, ambt.g, ambt.b);
   parsed.opacity = 1.0f;
   
   if (model == aiShadingMode_Gouraud)
   {
      parsed.specular = color();
      parsed.reflectivity = 0.0f;
   }
   else
   {
      parsed.reflectivity = 0.1f;
   }
   
   return parsed;
}

void Parser::ParseBones(aiVector3D *verts, aiBone** bones, unsigned int numBones)
{
   aiBone *currentBone; // The current bone in the array being transformed
   aiVertexWeight currentVertex; // The current vertex being affected
   unsigned int numVertices; // The number of vertices the current bone affects
   mat4 boneTransform; // The transform described by the current bone
   vec4 tempVertex; // Holds a representation of the current vertex which can be transformed by glm calls
   
   /* Iterate through all bones, and apply their transformation to the affected
    * vertices */
   for (int i = 0; i < numBones; i++)
   {
      currentBone = bones[i];
      numVertices = currentBone->mNumWeights;
      boneTransform = mat4(currentBone->mOffsetMatrix.a1, currentBone->mOffsetMatrix.a2,
                           currentBone->mOffsetMatrix.a3, currentBone->mOffsetMatrix.a4,
                           currentBone->mOffsetMatrix.b1, currentBone->mOffsetMatrix.b2,
                           currentBone->mOffsetMatrix.b3, currentBone->mOffsetMatrix.b4,
                           currentBone->mOffsetMatrix.c1, currentBone->mOffsetMatrix.c2,
                           currentBone->mOffsetMatrix.c3, currentBone->mOffsetMatrix.c4,
                           currentBone->mOffsetMatrix.d1, currentBone->mOffsetMatrix.d2,
                           currentBone->mOffsetMatrix.d3, currentBone->mOffsetMatrix.d4);
      
      /* Iterate through all affected vertices, applying the bones transformation
       * to them */
      for (int j = 0; j < numVertices; j++)
      {
         currentVertex = currentBone->mWeights[j];
         tempVertex = vec4(verts[currentVertex.mVertexId].x,
                           verts[currentVertex.mVertexId].y,
                           verts[currentVertex.mVertexId].z, 1.0f);
         tempVertex = tempVertex * (boneTransform * currentVertex.mWeight);
         
         verts[currentVertex.mVertexId].x = tempVertex.x;
         verts[currentVertex.mVertexId].y = tempVertex.y;
         verts[currentVertex.mVertexId].z = tempVertex.z;
      }
   }
}

mat4 Parser::TranslateNode(aiNodeAnim *node)
{
   mat4 transform = mat4(1.0f); // The matrix to be transformed and returned
   
   float preRatio; // The portion of the transformation to be determined by the previous key frame
   float postRatio; // The portion of the transformation to be determined by the next key frame
   
   aiVectorKey prePosition = aiVectorKey(-INF, aiVector3D()); // The translation given by the previous key frame
   aiVectorKey postPosition = aiVectorKey(INF, aiVector3D()); // The translation given by the next key frame
   
   /* Run through the key frames to determine which ones occur immediately before and after the given time */
   for (int i = 0; i < node->mNumPositionKeys; i++)
   {
      aiVectorKey tempPosition = node->mPositionKeys[i]; // Temporary storage for key frame candidate
      
      /* If the candidate occurs later than the current previous key while still occurring before the given time,
       * the candidate become the new previous key. */
      if (tempPosition.mTime > prePosition.mTime && tempPosition.mTime < time)
      {
         prePosition = tempPosition;
      }
      /* If the candidate occurs ealier than the current post key while still occurring after the given time,
       * the candidate become the new post key. */
      else if (tempPosition.mTime < postPosition.mTime && tempPosition.mTime > time)
      {
         postPosition = tempPosition;
      }
   }
   
   /* There was at least one key frame found, proceed */
   if (prePosition.mTime != -INF || postPosition.mTime != INF)
   {
      /* If no key frames occur after the given time, the previous key gets 100% of the weight */
      if (postPosition.mTime == INF)
      {
         preRatio = 1.0f;
         postRatio = 0.0f;
      }
      /* If no key frames occur before the given time, the post key gets 100% of the weight */
      else if (prePosition.mTime == -INF)
      {
         preRatio = 0.0f;
         postRatio = 1.0f;
      }
      /* Otherwise, the weights are computed by determine the position at the given time between the two key frames */
      else
      {
         float difference = postPosition.mTime - prePosition.mTime;
         postRatio = (time - prePosition.mTime) / difference;
         preRatio = 1.0f - postRatio;
      }
      
      /* The total translation is determined by mulitplying the ratios to their prospective translations and then summing them */
      vec3 trans = vec3(prePosition.mValue.x * preRatio + postPosition.mValue.x * postRatio,
                        prePosition.mValue.y * preRatio + postPosition.mValue.y * postRatio,
                        prePosition.mValue.z * preRatio + postPosition.mValue.z * postRatio);
      transform = transpose(translate(mat4(1.0f), trans)); // transform is set to the transformation matrix describing the translation
   }
   
   return transform;
}

mat4 Parser::ScaleNode(aiNodeAnim *node)
{
   mat4 transform = mat4(1.0f); // The matrix to be transformed and returned
   
   float preRatio; // The portion of the transformation to be determined by the previous key frame
   float postRatio; // The portion of the transformation to be determined by the next key frame
   
   aiVectorKey preScale = aiVectorKey(-INF, aiVector3D()); // The scale given by the previous key frame
   aiVectorKey postScale = aiVectorKey(INF, aiVector3D()); // The scale given by the next key frame
   
   /* Run through the key frames to determine which ones occur immediately before and after the given time */
   for (int i = 0; i < node->mNumScalingKeys; i++)
   {
      aiVectorKey tempScale = node->mScalingKeys[i]; // Temporary storage for key frame candidate
      
      /* If the candidate occurs later than the current previous key while still occurring before the given time,
       * the candidate become the new previous key. */
      if ((preScale.mTime == -INF || preScale.mTime < tempScale.mTime) && tempScale.mTime < time)
      {
         preScale = tempScale;
      }
      /* If the candidate occurs ealier than the current post key while still occurring after the given time,
       * the candidate become the new post key. */
      else if ((postScale.mTime == INF || postScale.mTime > tempScale.mTime) && tempScale.mTime > time)
      {
         postScale = tempScale;
      }
   }
   
   /* There was at least one key frame found, proceed */
   if (preScale.mTime != -INF || postScale.mTime != INF)
   {
      /* If no key frames occur after the given time, the previous key gets 100% of the weight */
      if (postScale.mTime == INF)
      {
         preRatio = 1.0f;
         postRatio = 0.0f;
      }
      /* If no key frames occur before the given time, the post key gets 100% of the weight */
      else if (preScale.mTime == -INF)
      {
         preRatio = 0.0f;
         postRatio = 1.0f;
      }
      /* Otherwise, the weights are computed by determine the scale at the given time between the two key frames */
      else
      {
         float difference = postScale.mTime - preScale.mTime;
         postRatio = (time - preScale.mTime) / difference;
         preRatio = 1.0f - postRatio;
      }
      
      /* The total scale is determined by mulitplying the ratios to their prospective scales and then summing them */
      vec3 scal = vec3(preScale.mValue.x * preRatio + postScale.mValue.x * postRatio,
                       preScale.mValue.y * preRatio + postScale.mValue.y * postRatio,
                       preScale.mValue.z * preRatio + postScale.mValue.z * postRatio);
      transform = transpose(scale(mat4(1.0f), scal)); // transform is set to the transformation matrix describing the scale
   }
   
   return transform;
}

mat4 Parser::RotateNode(aiNodeAnim *node)
{
   mat4 transform = mat4(1.0f);
   
   float preRatio; // The portion of the transformation to be determined by the previous key frame
   float postRatio; // The portion of the transformation to be determined by the next key frame

   aiQuatKey preRot = aiQuatKey(-INF, aiQuaternion()); // The rotation given by the previous key frame
   aiQuatKey postRot = aiQuatKey(INF, aiQuaternion()); // The scale given by the next key frame
   
   /* Run through the key frames to determine which ones occur immediately before and after the given time */
   for (int i = 0; i < node->mNumRotationKeys; i++)
   {
      aiQuatKey tempRot = node->mRotationKeys[i]; // Temporary storage for key frame candidate
      
      /* If the candidate occurs later than the current previous key while still occurring before the given time,
       * the candidate become the new previous key. */
      if ((preRot.mTime == -INF || preRot.mTime < tempRot.mTime) && tempRot.mTime < time)
      {
         preRot = tempRot;
      }
      /* If the candidate occurs ealier than the current post key while still occurring after the given time,
       * the candidate become the new post key. */
      else if ((postRot.mTime == INF || postRot.mTime > tempRot.mTime) && tempRot.mTime > time)
      {
         postRot = tempRot;
      }
   }
   
   /* There was at least one key frame found, proceed */
   if (preRot.mTime != -INF || postRot.mTime != INF)
   {
      /* If no key frames occur after the given time, the previous key gets 100% of the weight */
      if (postRot.mTime == INF)
      {
         preRatio = 1.0f;
         postRatio = 0.0f;
      }
      /* If no key frames occur before the given time, the post key gets 100% of the weight */
      else if (preRot.mTime == -INF)
      {
         preRatio = 0.0f;
         postRatio = 1.0f;
      }
      /* Otherwise, the weights are computed by determine the scale at the given time between the two key frames */
      else
      {
         float difference = postRot.mTime - preRot.mTime;
         postRatio = (time - preRot.mTime) / difference;
         preRatio = 1.0f - postRatio;
      }
      
      /* The total rotation is determined by mixing the previous and post rotations by their ratios */
      quat rot1 = quat(preRot.mValue.x, preRot.mValue.y, preRot.mValue.z, preRot.mValue.w);
      quat rot2 = quat(postRot.mValue.x, postRot.mValue.y, postRot.mValue.z, postRot.mValue.w);
      transform = transpose(mat4_cast(mix(rot1, rot2, preRatio))); // transform is set to the transformation matrix describing the rotation
   }
   
   return transform;
}

mat4 Parser::Transform()
{
   mat4 transform = mat4(1.0f); // The total transform over the hierarchy
   
   /* Multiply together all the transforms in the stack */
   for (int i = 0; i < transformStack->size(); i++) {
      transform *= transformStack->at(i);
   }
   
   return transform;
}