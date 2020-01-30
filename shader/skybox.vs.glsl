#version 400

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;


// get position from vertex array object
in vec3 pos;

in vec2 texCoords;

// send color to fragment shader
//out vec3 vcolor;

smooth out vec3 st;

void main(void)
{
    // calculate position in model view projection space
    gl_Position = projection_matrix * modelview_matrix * vec4(pos, 1);

    // Texture coordinates
    st = pos;


}
