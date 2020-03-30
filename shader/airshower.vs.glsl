#version 400

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

uniform vec3 col;
uniform vec3 camerapos;
uniform vec3 sfcenter;
uniform float time;
uniform float maxtime;
uniform float onlyFront;
uniform float stepLength;
uniform float pteffect;

//uniform float time;
//uniform float currTime;

// get position from vertex array object
in vec3 pos;
in float timestamp;


// send color to fragment shader
out vec3 vcolor;
out float valpha;

void main(void)
{
    // calculate position in model view projection space
    gl_Position = projection_matrix * modelview_matrix * vec4(pos, 1);

//    vcolor= vec3(1.0f,0.0f,0.0f);
    vcolor = col;

    if((time > timestamp && onlyFront<0.5) || ( abs(time-timestamp)<stepLength && onlyFront>=0.5) )
        valpha = 1.0f;
    else
        valpha = 0.0f;

// Penrose-Terrell effect

    float campartdist = distance(camerapos, pos); // distance between particle and camera
    float camsfcdist = distance(camerapos, sfcenter); // distance between camera and shower front center
    float c_vac = 3e5*maxtime; // approximate speed of light in vacuum
    float lighttime = (campartdist - camsfcdist)/c_vac;

    lighttime *= pteffect; //if pteffect is 0.0f(false) the penrose-terrell effect will be neglected

    if((time > timestamp+lighttime && onlyFront<0.5) || ( abs(time-(timestamp+lighttime))<stepLength && onlyFront>=0.5) )
        valpha = 1.0f;
    else
        valpha = 0.0f;
}
