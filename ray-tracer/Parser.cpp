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

#include "ray.h"

#define CAM_NAME "camera1"

Parser::Parser(const aiScene *scn)
{
   scene = scn; // Makes the given scene a member of the object for easy access
   time = 0.0f; // Initialize animations to time 0 to start
   
   transformStack = new vector<mat4>(); // Initialize the transform stack
   nodeTransforms = new unordered_map<string, mat4>(); // Initialize transform map
   meshBones = new unordered_map<string, bone_node *>(); // Initialize bone map
   
   lights = new vector<AbstractLight *>(); // Initialize the lights vector
   meshes = new vector<Mesh *>(); // Initialize the mesh vector
   
   aiNode *root = scn->mRootNode; // Get the root node to start traversing at
   
   /* For each child of the root node, gather transform information */
   for (int i = 0; i < root->mNumChildren; i++)
   {
      ParseTransformations(root->mChildren[i], NULL);
   }
   
   /* For each child of the root node, gather scene information */
   for (int i = 0; i < root->mNumChildren; i++)
   {
      NavigateNode(root->mChildren[i]);
   }
}

void Parser::UpdateScene(double t)
{
   aiNode *root = scene->mRootNode; // Get the root node to start traversing at
   time = t; // Set the time for animations to be resolved
   
   lights->clear(); // Empty the lights vector as it's about to be refilled
   meshes->clear(); // Do the same for teh meshes vector
   
   nodeTransforms = new unordered_map<string, mat4>(); // Initialize transform map
   meshBones = new unordered_map<string, bone_node *>(); // Initialize bone map
   
   /* For each child of the root node, gather transform information */
   for (int i = 0; i < root->mNumChildren; i++)
   {
      ParseTransformations(root->mChildren[i], NULL);
   }
   
   /* For each child of the root node, gather scene information */
   for (int i = 0; i < root->mNumChildren; i++)
   {
      NavigateNode(root->mChildren[i]);
   }
}


#pragma mark Private Functions

void Parser::ParseTransformations(aiNode *node, bone_node *parent)
{
   int numChildren = node->mNumChildren; // The number of children this node has
   mat4 transform = mat4(1.0f); // The transformation at this node
   
   aiAnimation *animation = NULL; // Temporary holder for animation candidate
   aiNodeAnim *nodeAnim = NULL; // The animation for this particular node
   aiNodeAnim *nodeTemp = NULL; // Temporary holder for node animation candidate
   
   bone_node *child = NULL;
   
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
   
   /* If this node is animated, calculate the change to the transformation for 
    * this node */
   if (nodeAnim)
   {
      transform = transform * RotateNode(nodeAnim);
      transform = transform * ScaleNode(nodeAnim);
      transform = transform * TranslateNode(nodeAnim);
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
   
   aiMesh *tempMesh;
   aiBone *tempBone;
   for (int i = 0; i < scene->mNumMeshes; i++)
   {
      tempMesh = scene->mMeshes[i];
      for (int j = 0; j < tempMesh->mNumBones; j++) {
         tempBone = tempMesh->mBones[j];
         if (tempBone->mName == node->mName)
         {
            child = new bone_node();
            
            if (parent == NULL)
            {
               child->transform = mat4(1.0f);
            }
            else
            {
               child->transform = transform;
            }
            
            child->boneSpace = mat4(tempBone->mOffsetMatrix.a1,
                                    tempBone->mOffsetMatrix.a2,
                                    tempBone->mOffsetMatrix.a3,
                                    tempBone->mOffsetMatrix.a4,
                                    tempBone->mOffsetMatrix.b1,
                                    tempBone->mOffsetMatrix.b2,
                                    tempBone->mOffsetMatrix.b3,
                                    tempBone->mOffsetMatrix.b4,
                                    tempBone->mOffsetMatrix.c1,
                                    tempBone->mOffsetMatrix.c2,
                                    tempBone->mOffsetMatrix.c3,
                                    tempBone->mOffsetMatrix.c4,
                                    tempBone->mOffsetMatrix.d1,
                                    tempBone->mOffsetMatrix.d2,
                                    tempBone->mOffsetMatrix.d3,
                                    tempBone->mOffsetMatrix.d4);
            child->parent = parent;
            
            pair<string, bone_node *> bone;
            bone = pair<string, bone_node *>(string(node->mName.C_Str()), child);
            meshBones->insert(bone);
         }
      }
   }
   
   pair<string, mat4> nodeTran;
   nodeTran = pair<string, mat4>(string(node->mName.C_Str()), Transform());
   nodeTransforms->insert(nodeTran);
   
   /* Now navigate through all the child nodes to collect their meshes */
   for (int i = 0; i < numChildren; i++)
   {
      ParseTransformations(node->mChildren[i], child);
   }
   
   /* Pop the current node's transformation off the stack so it doesn't affect 
    * other nodes */
   transformStack->pop_back();
}

void Parser::NavigateNode(aiNode *node)
{
   int numChildren = node->mNumChildren; // The number of children this node has
   
   /* Get transformation out of map */
   std::unordered_map<string,mat4>::const_iterator got;
   got = nodeTransforms->find(string(node->mName.C_Str()));
   
   mat4 transform = got->second; // The transformation at this node
   
   /* If the node contains a mesh, it creates a Mesh object representative of it
    * and adds it to meshes */
   if (node->mNumMeshes > 0)
   {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[0]]; // The mesh of the current node
      Mesh *myMesh = ParseMesh(mesh, transform);
      meshes->push_back(myMesh);
   }
   
   /* Loop through the list of cameras in the scene, if any of them share a name
    * with the current node create the camera to be used by ray-tracer */
   for (int i = 0; i < scene->mNumCameras; i++) {
      aiCamera *cam = scene->mCameras[i]; // The camera currently being inspected
      
      /* If the current camera has the same name as the current node, and is
       * named <CAM_NAME>, then the scene camera is created with the current 
       * node and camera's attributes */
      if (cam->mName == node->mName && cam->mName == aiString(CAM_NAME)) {
         vec3 eye = transform_point(vec3(cam->mPosition.x,
                                         cam->mPosition.y,
                                         cam->mPosition.z),
                                    transform); // The position of the camera
         vec3 lookAt = transform_point(vec3(cam->mLookAt.x,
                                            cam->mLookAt.y,
                                            cam->mLookAt.z),
                                       transform); // The focus of the camera
         
         camera = new class Camera(eye, lookAt, vec3(cam->mUp.x,
                                                     cam->mUp.y,
                                                     cam->mUp.z));
      }
   }
   
   /* Loop through the lights in the scene, if any of them share a name with the
    * current node create a light to be used by ray-tracer with the current 
    * attributes and transformation */
   for (int i = 0; i < scene->mNumLights; i++) {
      aiLight *lgt = scene->mLights[i]; // The light currently being inspected
      
      /* If the current light's name and the current node's name match, add the 
       * light to the scene */
      if (lgt->mName == node->mName) {
         vec3 pos = transform_point(vec3(lgt->mPosition.x,
                                         lgt->mPosition.y,
                                         lgt->mPosition.z), transform);
         color col = color(lgt->mColorDiffuse.r,
                           lgt->mColorDiffuse.g,
                           lgt->mColorDiffuse.b);
         
         /* The light to be added to the scene */
         PointLight *light = new PointLight(col, vec3(pos.x, pos.y, pos.z));
         lights->push_back(light);
      }
   }
   
   /* Now navigate through all the child nodes to collect their meshes */
   for (int i = 0; i < numChildren; i++)
   {
      NavigateNode(node->mChildren[i]);
   }
}

Mesh *Parser::ParseMesh(aiMesh *mesh, mat4 transform)
{
   Mesh *parsed; // The mesh parsed from the given aiMesh
   material mat; // The material of the parsed mesh
   
   aiVector3D *verts = mesh->mVertices; // The vertices that define the mesh
   aiVector3D *norms = mesh->mNormals; // The normals for the vertices
   /* The vector containing the faces of the mesh */
   vector<Intersectable<Renderable> *> meshTriangles;
   
   /* The vector of vertices to be transformed by bone deformations */
   vector<vec3> *vertices;
   vertices = new vector<vec3>(mesh->mNumVertices, vec3(0.0f));
   for (int i = 0; i < mesh->mNumVertices; i++)
   {
      vertices->at(i)= vec3(verts[i].x, verts[i].y, verts[i].z);
   }
   
   /* The vector of normals to be transformed by bone deformations */
   vector<vec3> *normals;
   normals = new vector<vec3>(mesh->mNumVertices, vec3(0.0f));
   for (int i = 0; i < mesh->mNumVertices; i++)
   {
      normals->at(i)= vec3(norms[i].x, norms[i].y, norms[i].z);
   }
   
   /* The mesh's material */
   aiMaterial *mater = scene->mMaterials[mesh->mMaterialIndex];
   mat = ParseMaterial(mater);
   
   if (mesh->HasBones())
   {
      ParseBones(vertices, normals,
                 mesh->mBones, mesh->mNumBones, mesh->mNumVertices);
   }
   
   /* For each face create a triangle and add it to meshTriangles */
   for (int j = 0; j < mesh->mNumFaces; j++) {
      aiFace face = mesh->mFaces[j]; // The current face
      
      /* If the given face has enough vertices to form a triangle, add it */
      for (int k = 0; k + 2 < face.mNumIndices; k++)
      {
         Triangle *tri = new Triangle(vertices->at(face.mIndices[k]),
                                      vertices->at(face.mIndices[k+1]),
                                      vertices->at(face.mIndices[k+2]),
                                      vec3(norms[face.mIndices[k]].x,
                                           norms[face.mIndices[k]].y,
                                           norms[face.mIndices[k]].z),
                                      vec3(norms[face.mIndices[k+1]].x,
                                           norms[face.mIndices[k+1]].y,
                                           norms[face.mIndices[k+1]].z),
                                      vec3(norms[face.mIndices[k+2]].x,
                                           norms[face.mIndices[k+2]].y,
                                           norms[face.mIndices[k+2]].z),
                                      mat);
         meshTriangles.push_back(tri);
      }
   }
   parsed = new Mesh(&meshTriangles, mat, transform); // Create a mesh from the given faces
   
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
   float refl; // The reflectiveness of the mesh
   float shin; // The shininess of the mesh
   
   
   /* extract the properties from the aiMaterial and put them into the material 
    * struct */
   aiReturn prop = mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
   prop = mat->Get(AI_MATKEY_COLOR_SPECULAR, spec);
   prop = mat->Get(AI_MATKEY_COLOR_AMBIENT, ambt);
   prop = mat->Get(AI_MATKEY_OPACITY, opac);
   prop = mat->Get(AI_MATKEY_SHADING_MODEL, model);
   prop = mat->Get(AI_MATKEY_REFLECTIVITY, refl);
   prop = mat->Get(AI_MATKEY_SHININESS, shin);
   parsed.diffuse = color(diff.r, diff.g, diff.b);
   parsed.specular = color(spec.r, spec.g, spec.b);
   parsed.ambient = color(ambt.r, ambt.g, ambt.b);
   parsed.opacity = 1.0f;
   parsed.shininess = shin;
   
//   cout << "Diffuse Color: (" << parsed.diffuse.r << ", "
//   << parsed.diffuse.g << ", " << parsed.diffuse.b << ")" << endl;
   
   if (model == aiShadingMode_Gouraud)
   {
      parsed.specular = color();
      parsed.reflectivity = 0.0f;
   }
   else
   {
      parsed.reflectivity = refl;
   }
   
   return parsed;
}

void Parser::ParseBones(vector<vec3> *vertices, vector<vec3> *normals,
                                 aiBone** bones, unsigned int numBones,
                                 unsigned int numVerts)
{
   /* The bone who's transformation is currently being calculated */
   aiBone *tempBone;
   /* The node from which the bone is gotten */
   bone_node *node;
   
   /* The index of the vertex being affected */
   aiVertexWeight tempVertex;
   
   /* The bone space transformation and animation transformation */
   mat4 transform;
   /* The inverse bone space transformation */
   mat4 invers;
   
   /* The summed transformation for each vertex */
   vector<mat4> vertexTransforms;
   vertexTransforms.resize(numVerts, mat4(0.0f));
   
   for (int i = 0; i < numBones; i++)
   {
      tempBone = bones[i];
      
      /* Get bone out of map */
      std::unordered_map<string, bone_node *>::const_iterator got;
      got = meshBones->find(string(tempBone->mName.C_Str()));
      node = got->second;
      
      transform = mat4(1.0f);
      /* Traverse each bone up the hierarchy (including this one), applying
       * its bone space and animation tranformations */
      for (bone_node *bone = node; bone != NULL; bone = bone->parent)
      {
         transform =  bone->boneSpace * bone->transform * transform;
      }
      
      /* Traverse each bone down the hierarchy (stopping at the current bone),
       * applying the inverse bone space tranformation. (maintaining the
       * animation transformation */
      invers = mat4(1.0f);
      for (bone_node *bone = node; bone != NULL; bone = bone->parent)
      {
         invers = invers * inverse(bone->boneSpace);
      }
      
      unsigned int numVertices = tempBone->mNumWeights;
      for (int i = 0; i < numVertices; i++)
      {
         tempVertex = tempBone->mWeights[i];
         
         if (tempVertex.mWeight > 0)
         {
            vertexTransforms[tempVertex.mVertexId] += (transform * invers)
            * tempVertex.mWeight;
         }
      }
   }
   
   /* Iterate through all affected vertices, applying the bone-space 
    * transformation to them */
   for (int i = 0; i < numVerts; i++)
   {
      vertices->at(i) = transform_point(vertices->at(i), vertexTransforms[i]);
   }
   
   /* Iterate through all affected normals, applying the bone-space 
    * transformation to them */
   for (int i = 0; i < numVerts; i++)
   {
      normals->at(i) = transform_normal(normals->at(i), vertexTransforms[i]);
   }
}

mat4 Parser::TranslateNode(aiNodeAnim *node)
{
   mat4 transform = mat4(1.0f); // The matrix to be transformed and returned
   
   /* The portion of the transformation determined by the previous key frame */
   float preRatio;
   /* The portion of the transformation determined by the next key frame */
   float postRatio;
   
   /* The translation given by the previous key frame */
   aiVectorKey prePosition = aiVectorKey(-INF, aiVector3D());
   /* The translation given by the next key frame */
   aiVectorKey postPosition = aiVectorKey(INF, aiVector3D());
   
   /* Run through the key frames to determine which ones occur immediately 
    * before and after the given time */
   for (int i = 0; i < node->mNumPositionKeys; i++)
   {
      /* Temporary storage for key frame candidate */
      aiVectorKey tempPosition = node->mPositionKeys[i];
      
      /* If the candidate occurs later than the current previous key while still 
       * occurring before the given time, the candidate become the new previous 
       * key. */
      if (tempPosition.mTime > prePosition.mTime && tempPosition.mTime < time)
      {
         prePosition = tempPosition;
      }
      /* If the candidate occurs ealier than the current post key while still 
       * occurring after the given time, the candidate become the new post 
       * key. */
      else if (tempPosition.mTime < postPosition.mTime && tempPosition.mTime > time)
      {
         postPosition = tempPosition;
      }
   }
   
   /* There was at least one key frame found, proceed */
   if (prePosition.mTime != -INF || postPosition.mTime != INF)
   {
      /* If no key frames occur after the given time, the previous key gets 100% 
       * of the weight */
      if (postPosition.mTime == INF)
      {
         preRatio = 1.0f;
         postRatio = 0.0f;
      }
      /* If no key frames occur before the given time, the post key gets 100% of
       * the weight */
      else if (prePosition.mTime == -INF)
      {
         preRatio = 0.0f;
         postRatio = 1.0f;
      }
      /* Otherwise, the weights are computed by determine the position at the 
       * given time between the two key frames */
      else
      {
         float difference = postPosition.mTime - prePosition.mTime;
         postRatio = (time - prePosition.mTime) / difference;
         preRatio = 1.0f - postRatio;
      }
      
      /* The total translation is determined by mulitplying the ratios to their 
       * prospective translations and then summing them */
      vec3 trans = vec3(prePosition.mValue.x * preRatio
                        + postPosition.mValue.x * postRatio,
                        prePosition.mValue.y * preRatio
                        + postPosition.mValue.y * postRatio,
                        prePosition.mValue.z * preRatio
                        + postPosition.mValue.z * postRatio);
      /* transform is set to the transformation matrix describing the translation */
      transform = transpose(translate(mat4(1.0f), trans));
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
   
   double preRatio; // The portion of the transformation to be determined by the previous key frame
   double postRatio; // The portion of the transformation to be determined by the next key frame

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
         double difference = postRot.mTime - preRot.mTime;
         postRatio = (time - preRot.mTime) / difference;
         preRatio = 1.0f - postRatio;
      }
      
      /* The total rotation is determined by mixing the previous and post rotations by their ratios */
      quat rot1 = quat(preRot.mValue.x, preRot.mValue.y, preRot.mValue.z, preRot.mValue.w);
      quat rot2 = quat(postRot.mValue.x, postRot.mValue.y, postRot.mValue.z, postRot.mValue.w);
      transform = transpose(mat4_cast(mix(rot1, rot2, (float)postRatio))); // transform is set to the transformation matrix describing the rotation
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