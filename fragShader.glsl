#version 430

//in vec2 TexCoords;
//out vec4 Tcolor; 

//uniform sampler2D text;
//uniform vec3 textColor;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix; 
uniform vec3 color;

void main(void)
{
//color = vec4(1.0, 0.0, 0.0, 1.0);
gl_FragColor = vec4(color, 1.0);
//vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
//Tcolor = vec4(textColor, 1.0) * sampled;
}