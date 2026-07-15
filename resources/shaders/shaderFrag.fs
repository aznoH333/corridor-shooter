#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;

void main()
{
    
	
	vec4 tex = texture(texture0, fragTexCoord);
    float crt = 1.0 - (sin(fragTexCoord.y * 600) * 0.4);
	tex.a = tex.a * crt;
	
	finalColor = tex * fragColor;
}

