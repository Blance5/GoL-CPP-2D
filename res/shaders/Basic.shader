#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

void main()
{
    gl_Position = position;
};



#shader fragment
#version 330 core

//out vec4 FragColor;

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
    //vec3 color = vec3(gl_FragCoord.x / 800.0, gl_FragCoord.y / 600.0, 0.7);
    color = u_Color;
    //FragColor = vec4(color, 1.0);
}