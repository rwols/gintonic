#pragma once

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
class matrix_PVM_shader;
class gp_shader;
class gp_d_shader;
class gp_s_shader;
class gp_n_shader;
class gp_ds_shader;
class gp_dn_shader;
class gp_sn_shader;
class gp_dsn_shader;
class gpi_shader;
class gpi_d_shader;
class gpi_s_shader;
class gpi_n_shader;
class gpi_ds_shader;
class gpi_dn_shader;
class gpi_sn_shader;
class gpi_dsn_shader;
class lp_ambient_shader;
class lp_directional_shader;
class lp_point_shader;
class lp_spot_shader;
class sp_directional_shader;
class skybox_shader;
class text_shader;

/* general classes */
class Entity;
class FbxImporter;
class Camera;

} // namespace gintonic