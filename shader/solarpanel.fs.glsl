#version 400

out vec4 fcolor;

smooth in vec2 st;
uniform sampler2D textur;

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

        vec3 normal = norm;
        //vec3 normal = vec3(0,1,0);
        vec3 showerLight= normalize(showerPosition-Mpos);

        vec3 view= normalize(-Mpos);
        vec3 r= (reflect(-view,normal));

        float distance = length(showerPosition-Mpos);
        float lightFluxNormalization = 100.0f;
        float lightFlux = lightFluxNormalization / (distance*distance);
        if(lightFlux > 1.f) lightFlux = 1.f;

        //ambient lighting
        vec3 ambient= ka*La;
        //diffuse lighting
        vec3 diffuse= lightFlux *  kd*Ld* max(dot(showerLight,normal),0.0);

        //specular lighting

        vec3 spec= lightFlux *  ks*Ls*pow(max(dot(r,showerLight),0.0),shininess);

        vec4 texCol = texture2D(textur, st);
        vec3 color = (ambient+diffuse+spec);

        fcolor = (texCol*vec4(color,1));
}
