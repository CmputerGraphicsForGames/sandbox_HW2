#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec4 varyingColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    //FragColor = texture(ourTexture, TexCoord);
    FragColor = vec4(ourColor,1.0f);
    //FragColor = varyingColor;
}



















