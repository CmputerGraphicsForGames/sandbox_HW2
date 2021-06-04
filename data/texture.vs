#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec4 varyingColor;

uniform float offset;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	gl_Position.x += offset;
	gl_Position.y += offset;
	gl_Position.z += offset;
	
	ourColor = vec3(1.0f,0.0f,0.0f);
	varyingColor = vec4(aPos,1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);

    TexCoord = aTexCoord;
}



















