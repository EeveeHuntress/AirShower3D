#version 400

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

// get position from vertex array object
layout(location = 0) in vec3 vpos;
layout(location=1) in vec3 vnorm;

// send color to fragment shader
out vec3 vcolor;
out vec3 pos;
out vec3 norm;

void main(void)
{
    // calculate position in model view projection space
    gl_Position = projection_matrix * modelview_matrix * vec4(vpos, 1);

    pos=vpos;
    norm=vnorm;


    vcolor = vec3(1.0f,0.9f,0.6f);
}
