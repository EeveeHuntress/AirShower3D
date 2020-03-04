#version 400

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

uniform vec3 col;

//uniform float time;
//uniform float currTime;

// get position from vertex array object
in vec3 pos;


// send color to fragment shader
out vec3 vcolor;
//out float vtime;

void main(void)
{
    // calculate position in model view projection space
    gl_Position = projection_matrix * modelview_matrix * vec4(pos, 1);

//    vcolor= vec3(1.0f,0.0f,0.0f);
    vcolor = col;
}
