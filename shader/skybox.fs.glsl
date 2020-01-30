#version 400
#extension GL_NV_shadow_samplers_cube : enable

out vec4 fcolor;

smooth in vec3 st;
uniform samplerCube skybox;

void main(void)
{
    fcolor= textureCube(skybox,st);
}
