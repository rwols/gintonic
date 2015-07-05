//
// phong_brdf.fs
//

#version 330 core

//precision mediump float; 

in vec3 v_position;
in vec2 v_uv;
in vec3 v_normal;

uniform int mode;

uniform vec4      ambient_factor;
uniform vec4      diffuse_factor;
uniform sampler2D diffuse_texture;
uniform float     normal_factor;
uniform sampler2D normal_texture;
uniform float     specular_shininess;
uniform sampler2D specular_texture;

//uniform vec3      light_direction; // normalized (!!!) light direction

const vec3 light_position = vec3(1.0f, 1.0f, 1.0f);
const vec3 specular_color = vec3(1.0f, 1.0f, 1.0f);

out vec4 final_color;

void main()
{
    vec4 diffuse_tex = texture(diffuse_texture, v_uv);
    

    vec3 normal = normalize(v_normal); 
    vec3 light_dir = normalize(light_position - v_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    vec3 view_dir = normalize(-v_position);
    float specular_angle = max(dot(reflect_dir, view_dir), 0.0f);

    vec4 ambient_color = ambient_factor * diffuse_tex;
    vec4 diffuse_color = max(dot(light_dir,normal), 0.0f) * diffuse_factor * diffuse_tex;
    vec4 specular_color = pow(specular_angle, specular_shininess) * vec4(1.0f, 1.0f, 1.0f, 1.0f);

    final_color = ambient_color + diffuse_color + specular_color;
}
