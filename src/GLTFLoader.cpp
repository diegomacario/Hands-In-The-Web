#pragma warning(disable : 26812)

#include <iostream>

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"

#include "Transform.h"
#include "GLTFLoader.h"

namespace GLTFHelpers
{
   // In a glTF file...
   // - A buffer contains data that is read from a file
   //   Properties: byteLength, uri
   // - A bufferView defines a segment of the buffer data
   //   Properties: buffer, byteOffset, byteLength, byteStride, target (optional OpenGL buffer target)
   // - An accessor defines how the data of a bufferView is interpreted
   //   Properties: bufferView, byteOffset, type (e.g. "VEC2"), componentType (e.g. GL_FLOAT), count (e.g. number of "VEC2"s in the bufferView), min, max
   // The function below reads the values of an accessor whose componentType must be GL_FLOAT
   // E.g. For an accessor with a type of "VEC2", a componentType of GL_FLOAT and a count of 32, componentCount should be equal to 2,
   //      which would result in 64 floats being read
   void GetFloatsFromAccessor(const cgltf_accessor& accessor, unsigned int componentCount, std::vector<float>& outValues)
   {
      outValues.resize(accessor.count * componentCount);

      for (cgltf_size i = 0; i < accessor.count; ++i)
      {
         cgltf_accessor_read_float(&accessor, i, &outValues[i * componentCount], componentCount);
      }
   }

   // A glTF file may contain an array of meshes
   // Each mesh may contain multiple mesh primitives, which refer to the geometry data that is required to render a mesh
   // Each mesh primitive consists of:
   // - A rendering mode (e.g. POINTS, LINES or TRIANGLES)
   // - A set of indices
   // - The attributes of the vertices
   // - The material that should be used for rendering
   // Each attribute is defined by mapping the attribute name (e.g. "POSITION", "NORMAL", etc.)
   // to the index of the accessor that contains the attribute data
   void StoreValuesOfAttributeInStaticMesh(cgltf_attribute& attribute, StaticMesh& outMesh)
   {
      // Get the accessor of the attribute and its component count
      cgltf_accessor& accessor = *attribute.data;
      unsigned int componentCount = 0;
      if (accessor.type == cgltf_type_vec2)
      {
         componentCount = 2;
      }
      else if (accessor.type == cgltf_type_vec3)
      {
         componentCount = 3;
      }
      else if (accessor.type == cgltf_type_vec4)
      {
         componentCount = 4;
      }

      // Read the floats from the accessor
      std::vector<float> attributeFloats;
      GetFloatsFromAccessor(accessor, componentCount, attributeFloats);

      // Get the vectors that will store the attributes of the mesh that we are loading
      // In each call to this function we only fill one of these, since a cgltf_attribute only describes one attribute
      std::vector<glm::vec3>&  positions  = outMesh.GetPositions();
      std::vector<glm::vec3>&  normals    = outMesh.GetNormals();
      std::vector<glm::vec2>&  texCoords  = outMesh.GetTexCoords();

      // Loop over all the values of the accessor
      // Note that accessor.count is not equal to the number of floats in the accessor
      // It's equal to the number of attribute values (e.g, vec2s, vec3s, vec4s, etc.) in the accessor
      cgltf_attribute_type attributeType = attribute.type;
      unsigned int numAttributeValues = static_cast<unsigned int>(accessor.count);
      for (unsigned int attributeValueIndex = 0; attributeValueIndex < numAttributeValues; ++attributeValueIndex)
      {
         // Store the current attribute value in the correct vector of the mesh
         // TODO: This looks inefficient. I think it would be better to have separate for loops inside if-statements
         //       Since this function only loads one attribute type when it's called, it doesn't make sense to check
         //       the attribute type in each iteration of this loop
         int indexOfFirstFloatOfCurrAttribValue = attributeValueIndex * componentCount;
         switch (attributeType)
         {
         case cgltf_attribute_type_position:
            // Store a position vec3
            positions.push_back(glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                          attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                          attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]));
            break;
         case cgltf_attribute_type_texcoord:
            // Store a texture coordinates vec2
            texCoords.push_back(glm::vec2(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                          attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1]));
            break;
         case cgltf_attribute_type_normal:
         {
            // Store a normal vec3
            glm::vec3 normal = glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                         attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                         attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]);

            // TODO: Use a constant here and add an error message
            if (glm::length2(normal) < 0.000001f)
            {
               normal = glm::vec3(0, 1, 0);
            }

            normals.push_back(glm::normalize(normal));
         }
         break;
         case cgltf_attribute_type_invalid:
         case cgltf_attribute_type_tangent:
         case cgltf_attribute_type_color:
         case cgltf_attribute_type_joints:
         case cgltf_attribute_type_weights:
            break;
         }
      }
   }

   // This function is identical to the one above, except that it's tailored for meshes that we will never render
   void StoreValuesOfAttributeInSimpleMesh(cgltf_attribute& attribute, SimpleMesh& outMesh)
   {
      // Get the accessor of the attribute and its component count
      cgltf_accessor& accessor = *attribute.data;
      unsigned int componentCount = 0;
      if (accessor.type == cgltf_type_vec2)
      {
         componentCount = 2;
      }
      else if (accessor.type == cgltf_type_vec3)
      {
         componentCount = 3;
      }
      else if (accessor.type == cgltf_type_vec4)
      {
         componentCount = 4;
      }

      // Read the floats from the accessor
      std::vector<float> attributeFloats;
      GetFloatsFromAccessor(accessor, componentCount, attributeFloats);

      // Get the vectors that will store the attributes of the mesh that we are loading
      // In each call to this function we only fill one of these, since a cgltf_attribute only describes one attribute
      std::vector<glm::vec3>&  positions  = outMesh.GetPositions();
      std::vector<glm::vec3>&  normals    = outMesh.GetNormals();
      std::vector<glm::vec2>&  texCoords  = outMesh.GetTexCoords();

      // Loop over all the values of the accessor
      // Note that accessor.count is not equal to the number of floats in the accessor
      // It's equal to the number of attribute values (e.g, vec2s, vec3s, vec4s, etc.) in the accessor
      cgltf_attribute_type attributeType = attribute.type;
      unsigned int numAttributeValues = static_cast<unsigned int>(accessor.count);
      for (unsigned int attributeValueIndex = 0; attributeValueIndex < numAttributeValues; ++attributeValueIndex)
      {
         // Store the current attribute value in the correct vector of the mesh
         // TODO: This looks inefficient. I think it would be better to have separate for loops inside if-statements
         //       Since this function only loads one attribute type when it's called, it doesn't make sense to check
         //       the attribute type in each iteration of this loop
         int indexOfFirstFloatOfCurrAttribValue = attributeValueIndex * componentCount;
         switch (attributeType)
         {
         case cgltf_attribute_type_position:
            // Store a position vec3
            positions.push_back(glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                          attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                          attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]));
            break;
         case cgltf_attribute_type_texcoord:
            // Store a texture coordinates vec2
            texCoords.push_back(glm::vec2(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                          attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1]));
            break;
         case cgltf_attribute_type_normal:
         {
            // Store a normal vec3
            glm::vec3 normal = glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                         attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                         attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]);

            // TODO: Use a constant here and add an error message
            if (glm::length2(normal) < 0.000001f)
            {
               normal = glm::vec3(0, 1, 0);
            }

            normals.push_back(glm::normalize(normal));
         }
         break;
         case cgltf_attribute_type_invalid:
         case cgltf_attribute_type_tangent:
         case cgltf_attribute_type_color:
         case cgltf_attribute_type_joints:
         case cgltf_attribute_type_weights:
            break;
         }
      }
   }
}

cgltf_data* LoadGLTFFile(const char* path)
{
   // The cgltf_data struct contains all the information that's stored in a glTF file:
   // scenes, nodes, meshes, materials, skins, animations, cameras, textures, images, samplers, buffers, bufferViews and accessors
   cgltf_data* data = nullptr;

   // The cgltf_options struct can be used to control parts of the parsing process
   // Initializing it to zero tells cgltf that we want to use the default behavior
   cgltf_options options;
   memset(&options, 0, sizeof(cgltf_options));

   // Open the glTF file and parse the glTF data
   cgltf_result result = cgltf_parse_file(&options, path, &data);
   if (result != cgltf_result_success)
   {
      std::cout << "Could not parse the following glTF file: " << path << '\n';
      return nullptr;
   }

   // Open and read the buffer files referenced by the glTF file
   // Buffer files are binary files that contain geometry or animation data
   result = cgltf_load_buffers(&options, data, path);
   if (result != cgltf_result_success)
   {
      cgltf_free(data);
      std::cout << "Could not load the buffers of the following glTF file: " << path << '\n';
      return nullptr;
   }

   // Verify that the parsed glTF data is valid
   result = cgltf_validate(data);
   if (result != cgltf_result_success)
   {
      cgltf_free(data);
      std::cout << "The following glTF file is invalid: " << path << '\n';
      return nullptr;
   }

   return data;
}

void FreeGLTFFile(cgltf_data* data)
{
   if (data)
   {
      cgltf_free(data);
   }
   else
   {
      std::cout << "Tried to call cgltf_free on a null cgltf_data pointer\n";
   }
}

// This function is identical to the one above, except that it loads the meshes of nodes that don't refer to skins
// In other words, it loads static meshes
std::vector<StaticMesh> LoadStaticMeshes(cgltf_data* data)
{
   std::vector<StaticMesh> staticMeshes;

   // Loop over the array of nodes of the glTF file
   unsigned int numNodes = static_cast<unsigned int>(data->nodes_count);
   for (unsigned int nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
   {
      // This function only loads static meshes, so the node must contain a mesh for us to process it
      cgltf_node* currNode = &data->nodes[nodeIndex];
      if (currNode->mesh == nullptr)
      {
         continue;
      }

      // Loop over the array of mesh primitives of the current node
      unsigned int numPrimitives = static_cast<unsigned int>(currNode->mesh->primitives_count);
      for (unsigned int primitiveIndex = 0; primitiveIndex < numPrimitives; ++primitiveIndex)
      {
         // Get the current mesh primitive
         cgltf_primitive* currPrimitive = &currNode->mesh->primitives[primitiveIndex];

         // Create a StaticMesh for the current mesh primitive
         staticMeshes.push_back(StaticMesh());
         StaticMesh& currMesh = staticMeshes[staticMeshes.size() - 1];

         // Loop over the attributes of the current mesh primitive
         unsigned int numAttributes = static_cast<unsigned int>(currPrimitive->attributes_count);
         for (unsigned int attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
         {
            // Get the current attribute
            cgltf_attribute* attribute = &currPrimitive->attributes[attributeIndex];
            // Read the values of the current attribute and store them in the current mesh
            GLTFHelpers::StoreValuesOfAttributeInStaticMesh(*attribute, currMesh);
         }

         // If the current mesh primitive has a set of indices, store them too
         if (currPrimitive->indices != nullptr)
         {
            unsigned int indexCount = static_cast<unsigned int>(currPrimitive->indices->count);

            // Get the vector that will store the indices of the current mesh
            std::vector<unsigned int>& indices = currMesh.GetIndices();
            indices.resize(indexCount);

            // Loop over the indices of the current mesh primitive
            for (unsigned int i = 0; i < indexCount; ++i)
            {
               indices[i] = static_cast<unsigned int>(cgltf_accessor_read_index(currPrimitive->indices, i));
            }
         }

         // TODO: Perhaps we shouldn't do this here. The user should choose when this is done
         // Once we are done loading the current mesh, we load its VBOs with the data that we read
         currMesh.LoadBuffers();
      }
   }

   return staticMeshes;
}

// This function is identical to the one above, except that it loads the meshes of nodes that we will never render
std::vector<SimpleMesh> LoadSimpleMeshes(cgltf_data* data)
{
   std::vector<SimpleMesh> simpleMeshes;

   // Loop over the array of nodes of the glTF file
   unsigned int numNodes = static_cast<unsigned int>(data->nodes_count);
   for (unsigned int nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
   {
      // This function only loads static meshes, so the node must contain a mesh for us to process it
      cgltf_node* currNode = &data->nodes[nodeIndex];
      if (currNode->mesh == nullptr)
      {
         continue;
      }

      // Loop over the array of mesh primitives of the current node
      unsigned int numPrimitives = static_cast<unsigned int>(currNode->mesh->primitives_count);
      for (unsigned int primitiveIndex = 0; primitiveIndex < numPrimitives; ++primitiveIndex)
      {
         // Get the current mesh primitive
         cgltf_primitive* currPrimitive = &currNode->mesh->primitives[primitiveIndex];

         // Create a SimpleMesh for the current mesh primitive
         simpleMeshes.push_back(SimpleMesh());
         SimpleMesh& currMesh = simpleMeshes[simpleMeshes.size() - 1];

         // Loop over the attributes of the current mesh primitive
         unsigned int numAttributes = static_cast<unsigned int>(currPrimitive->attributes_count);
         for (unsigned int attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
         {
            // Get the current attribute
            cgltf_attribute* attribute = &currPrimitive->attributes[attributeIndex];
            // Read the values of the current attribute and store them in the current mesh
            GLTFHelpers::StoreValuesOfAttributeInSimpleMesh(*attribute, currMesh);
         }

         // If the current mesh primitive has a set of indices, store them too
         if (currPrimitive->indices != nullptr)
         {
            unsigned int indexCount = static_cast<unsigned int>(currPrimitive->indices->count);

            // Get the vector that will store the indices of the current mesh
            std::vector<unsigned int>& indices = currMesh.GetIndices();
            indices.resize(indexCount);

            // Loop over the indices of the current mesh primitive
            for (unsigned int i = 0; i < indexCount; ++i)
            {
               indices[i] = static_cast<unsigned int>(cgltf_accessor_read_index(currPrimitive->indices, i));
            }
         }
      }
   }

   return simpleMeshes;
}
