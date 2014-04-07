//
//  Parser.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 4/2/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Parser.h"

#define CAM_NAME "turnTableCamera1"
#define INF 10000.0

Parser::Parser(const aiScene *scn)
{
   scene = scn;
   
   lights = new vector<AbstractLight *>();
   meshes = new vector<Mesh *>();
   
   aiNode *root = scn->mRootNode;
   
   for (int i = 0; i < root->mNumChildren; i++) {
      vector<Mesh *> *subChildren = NavigateNode(root->mChildren[i], 0.0f);
      for (int j = 0; j < subChildren->size(); j++)
      {
         meshes->push_back(subChildren->at(j));
      }
   }
}

void Parser::UpdateScene(float time)
{
   aiNode *root = scene->mRootNode;
   
   lights->clear();
   meshes->clear();
   
   for (int i = 0; i < root->mNumChildren; i++) {
      vector<Mesh *> *subChildren = NavigateNode(root->mChildren[i], time);
      for (int j = 0; j < subChildren->size(); j++)
      {
         meshes->push_back(subChildren->at(j));
      }
   }
}

vector<Mesh *> *Parser::NavigateNode(aiNode *node, float time)
{
   vector<Mesh *> *children = new vector<Mesh *>();
   
   int numChildren = node->mNumChildren;
   mat4 transform = mat4(node->mTransformation.a1, node->mTransformation.a2,
                              node->mTransformation.a3, node->mTransformation.a4,
                              node->mTransformation.b1, node->mTransformation.b2,
                              node->mTransformation.b3, node->mTransformation.b4,
                              node->mTransformation.c1, node->mTransformation.c2,
                              node->mTransformation.c3, node->mTransformation.c4,
                              node->mTransformation.d1, node->mTransformation.d2,
                              node->mTransformation.d3, node->mTransformation.d4);
   
   aiAnimation *animation = NULL;
   aiNodeAnim *nodeAnim = NULL;
   aiNodeAnim *nodeTemp = NULL;
   
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
   
   if (nodeAnim)
   {
      float preRatio;
      float postRatio;
      
      aiVectorKey prePosition = aiVectorKey(-INF, aiVector3D());
      aiVectorKey postPosition = aiVectorKey(INF, aiVector3D());
      for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
      {
         aiVectorKey tempPosition = nodeAnim->mPositionKeys[i];
         if ((prePosition.mTime == -INF || prePosition.mTime < tempPosition.mTime) && tempPosition.mTime < time)
         {
            prePosition = tempPosition;
         }
         else if ((postPosition.mTime == INF || postPosition.mTime > tempPosition.mTime) && tempPosition.mTime > time)
         {
            postPosition = tempPosition;
         }
      }
      
      if (prePosition.mTime != -INF || postPosition.mTime != INF)
      {
         if (postPosition.mTime == INF)
         {
            preRatio = 1.0f;
            postRatio = 0.0f;
         }
         else if (prePosition.mTime == -INF)
         {
            preRatio = 0.0f;
            postRatio = 1.0f;
         }
         else
         {
            float difference = postPosition.mTime - prePosition.mTime;
            preRatio = (time - prePosition.mTime) / difference;
            postRatio = 1.0f - preRatio;
         }
         
         vec3 trans = vec3(prePosition.mValue.x * preRatio + postPosition.mValue.x * postRatio,
                           prePosition.mValue.y * preRatio + postPosition.mValue.y * postRatio,
                           prePosition.mValue.z * preRatio + postPosition.mValue.z * postRatio);
         transform = transform * transpose(translate(mat4(1.0f), trans));
      }
      
      aiVectorKey preScale = aiVectorKey(-INF, aiVector3D());
      aiVectorKey postScale = aiVectorKey(INF, aiVector3D());
      for (int i = 0; i < nodeAnim->mNumScalingKeys; i++)
      {
         aiVectorKey tempScale = nodeAnim->mScalingKeys[i];
         if ((preScale.mTime == -INF || preScale.mTime < tempScale.mTime) && tempScale.mTime < time)
         {
            preScale = tempScale;
         }
         else if ((postScale.mTime == INF || postScale.mTime > tempScale.mTime) && tempScale.mTime > time)
         {
            postScale = tempScale;
         }
      }
      
      if (preScale.mTime != -INF || postScale.mTime != INF)
      {
         if (postScale.mTime == INF)
         {
            preRatio = 1.0f;
            postRatio = 0.0f;
         }
         else if (preScale.mTime == -INF)
         {
            preRatio = 0.0f;
            postRatio = 1.0f;
         }
         else
         {
            float difference = postScale.mTime - preScale.mTime;
            preRatio = (time - preScale.mTime) / difference;
            postRatio = 1.0f - preRatio;
         }
         
         vec3 scal = vec3(preScale.mValue.x * preRatio + postScale.mValue.x * postRatio,
                          preScale.mValue.y * preRatio + postScale.mValue.y * postRatio,
                          preScale.mValue.z * preRatio + postScale.mValue.z * postRatio);
         transform = transform * transpose(scale(mat4(1.0f), scal));
      }
      
      aiQuatKey preRot = aiQuatKey(-INF, aiQuaternion());
      aiQuatKey postRot = aiQuatKey(INF, aiQuaternion());
      for (int i = 0; i < nodeAnim->mNumRotationKeys; i++)
      {
         aiQuatKey tempRot = nodeAnim->mRotationKeys[i];
         if ((preRot.mTime == -INF || preRot.mTime < tempRot.mTime) && tempRot.mTime < time)
         {
            preRot = tempRot;
         }
         else if ((postRot.mTime == INF || postRot.mTime > tempRot.mTime) && tempRot.mTime > time)
         {
            postRot = tempRot;
         }
      }
      
      if (preRot.mTime != -INF || postRot.mTime != INF)
      {
         if (postRot.mTime == INF)
         {
            preRatio = 1.0f;
            postRatio = 0.0f;
         }
         else if (preRot.mTime == -INF)
         {
            preRatio = 0.0f;
            postRatio = 1.0f;
         }
         else
         {
            float difference = postRot.mTime - preRot.mTime;
            preRatio = (time - preRot.mTime) / difference;
            postRatio = 1.0f - preRatio;
         }
         
         quat rot1 = quat(preRot.mValue.x, preRot.mValue.y, preRot.mValue.z, preRot.mValue.w);
         quat rot2 = quat(postRot.mValue.x, postRot.mValue.y, postRot.mValue.z, postRot.mValue.w);
         mat4 rot = transpose(mat4_cast(mix(rot1, rot2, preRatio)));
         transform = transform * rot;
      }
   }
   
   /*
    * Loops through the meshes contained in this node and adds them to
    * "triangles".
    */
   for (int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; // The current mesh
      aiVector3D *verts = mesh->mVertices; // The vertices that define the mesh
      aiVector3D *norms = mesh->mNormals;
      vector<Intersectable<Renderable> *> meshTriangles;
      material mat = material();
      for (int j = 0; j < mesh->mNumFaces; j++) {
         aiFace face = mesh->mFaces[j]; // The current face
         if (face.mNumIndices == 3) {
            aiMaterial *mater = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D diff(0.0f, 0.0f, 0.0f);
            aiColor3D spec(0.0f, 0.0f, 0.0f);
            aiColor3D ambt(0.0f, 0.0f, 0.0f);
            float opac;
            aiReturn prop = mater->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
            prop = mater->Get(AI_MATKEY_COLOR_SPECULAR, spec);
            prop = mater->Get(AI_MATKEY_COLOR_AMBIENT, ambt);
            prop = mater->Get(AI_MATKEY_OPACITY, opac);
            mat.diffuse = color(diff.r, diff.g, diff.b);
            mat.specular = color(spec.r, spec.g, spec.b);
            mat.ambient = color(ambt.r, ambt.g, ambt.b);
            mat.opacity = opac;
            
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
      Mesh *myMesh = new Mesh(&meshTriangles, mat, transform);
      children->push_back(myMesh);
   }
   
   /*
    * Loops through the aiCameras contained by "scene" to see if any match the
    * name of the current node.
    */
   for (int i = 0; i < scene->mNumCameras; i++) {
      aiCamera *cam = scene->mCameras[i];
      if (cam->mName == node->mName && cam->mName == aiString(CAM_NAME)) {
         vec3 eye = transform_point(vec3(cam->mPosition.x, cam->mPosition.y, cam->mPosition.z), transform);
         vec3 lookAt = transform_point(vec3(cam->mLookAt.x, cam->mLookAt.y, cam->mLookAt.z), mat4(1.0f));
         
         printf("Camera: (%0.3f, %0.3f, %0.3f)\n", eye.x, eye.y, eye.z);
         printf("Look At: (%0.3f, %0.3f, %0.3f)\n", lookAt.x, lookAt.y, lookAt.z);
         
         camera = new class Camera(eye,
                              lookAt,
                              vec3(cam->mUp.x, cam->mUp.y, cam->mUp.z));
      }
   }
   
   /*
    * Loops through the aiLights contained by "scene" to see if any match the
    * name of the current node, adding them to "lights" if so.
    */
   for (int i = 0; i < scene->mNumLights; i++) {
      aiLight *lgt = scene->mLights[i];
      if (lgt->mName == node->mName) {
         vec4 pos = vec4(lgt->mPosition.x,
                         lgt->mPosition.y,
                         lgt->mPosition.z,
                         1.0f) * transform;
         color col = color(lgt->mColorDiffuse.r,
                           lgt->mColorDiffuse.g,
                           lgt->mColorDiffuse.b);
         
         PointLight *light = new PointLight(col, vec3(pos.x, pos.y, pos.z));
         lights->push_back(light);
      }
   }
   
   if (children->size() == 0)
   {
      children->push_back(new Mesh(transform));
   }
   
   for (int i = 0; i < numChildren; i++) {
      vector<Mesh *> *subChildren = NavigateNode(node->mChildren[i], time);
      children->at(0)->AddChildren(subChildren);
   }
   
   return children;
}