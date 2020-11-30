#version 400

// get color from vertex shader
in vec3 vcolor;

// send color to screen
layout(location = 0) out vec4 fcolor;

in vec3 pos;
in vec3 norm;

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform vec3 showerPosition;

uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 ka;
uniform float shininess;

void main(void)
{
    //lighting

        //sun
        vec3 Mpos= pos;

        vec3 normal = /*normalize*/(norm);
        //vec3 normal = vec3(0,1,0);
        vec3 showerLight= normalize(showerPosition-Mpos);

        vec3 view= normalize(-Mpos);
        vec3 r= (reflect(-view,normal));

        float distance = length(showerPosition-Mpos);
        float lightFluxNormalization = 100.0f;
        float lightFlux = lightFluxNormalization / (distance*distance);

        //ambient lighting
        vec3 ambient= ka*La;
        //diffuse lighting
        vec3 diffuse= lightFlux * kd*Ld* max(dot(showerLight,normal),0.0);

        //specular lighting

        vec3 spec= lightFlux * ks*Ls*pow(max(dot(r,showerLight),0.0),shininess);
        vec3 color = (ambient+diffuse+spec);

        fcolor = vec4(vcolor,1)*(vec4(color,1));
}
