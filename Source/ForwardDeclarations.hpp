#pragma once

#include <cstddef>

/* FBX SDK classes */
#include <fbxsdk/fbxsdk_nsbegin.h>
class FbxVector2;
class FbxVector4;
class FbxObject;
class FbxNode;
class FbxImporter;
class FbxScene;
class FbxManager;
class FbxMesh;
class FbxNodeAttribute;
class FbxSurfaceMaterial;
class FbxLight;
class FbxCamera;
#include <fbxsdk/fbxsdk_nsend.h>

namespace gintonic {

/* Foundation classes */
template <class Derived, class NameType> class Object;
template <class T, std::size_t Alignment> class allocator;
class exception;
class ReadLock;
class ReadWriteLock;
class timer;
class one_shot_timer;
class loop_timer;

/* Math classes */
union vec2f;
union vec3f;
union vec4f;
union quatf;
union mat2f;
union mat3f;
union mat4f;
struct SQT;
struct box2f;
struct box3f;
class MatrixPipeline;
class mat4fstack;
class SQTstack;

/* Graphics classes */
class DirectionalShadowBuffer;
class Font;
class Mesh;
class PointShadowBuffer;
class ShadowBuffer;
class Texture2D;
class Light;
class AmbientLight;
class DirectionalLight;
class PointLight;
class SpotLight;
class Material;
class Renderer;
class skybox;
class unit_quad_P;
class unit_cube_P;
class unit_cube_P_flipped;
class unit_sphere_P;
class unit_cone_P;
class Mesh;
class Matrix_PVM_ShaderProgram;
class GeometryShaderProgram;
class Geometry_D_ShaderProgram;
class Geometry_S_ShaderProgram;
class Geometry_N_ShaderProgram;
class Geometry_DS_ShaderProgram;
class Geometry_DN_ShaderProgram;
class Geometry_SN_ShaderProgram;
class Geometry_DSN_ShaderProgram;
class GeometryInstancedShaderProgram;
class GeometryInstanced_D_ShaderProgram;
class GeometryInstanced_S_ShaderProgram;
class GeometryInstanced_N_ShaderProgram;
class GeometryInstanced_DS_ShaderProgram;
class GeometryInstanced_DN_ShaderProgram;
class GeometryInstanced_SN_ShaderProgram;
class GeometryInstanced_DSN_ShaderProgram;
class AmbientLightShaderProgram;
class DirectionalLightShaderProgram;
class PointLightShaderProgram;
class SpotLightShaderProgram;
class sp_directional_shader;
class SkyboxShaderProgram;
class FlatTextShaderProgram;

/* general classes */
class Entity;
class FbxImporter;
class Camera;

} // namespace gintonic