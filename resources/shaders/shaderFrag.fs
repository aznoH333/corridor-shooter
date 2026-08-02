#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;

const float RENDER_WIDTH = 200.0;
const float RENDER_HEIGHT = 100.0;

vec2 pixelizeTexCoord(vec2 texCoord)
{
    vec2 renderResolution = vec2(RENDER_WIDTH, RENDER_HEIGHT);
    return (floor(texCoord * renderResolution) + 0.5) / renderResolution;
}

void main()
{
    
	
    vec2 pixelTexCoord = fragTexCoord;//pixelizeTexCoord(fragTexCoord);
	vec4 tex = texture(texture0, pixelTexCoord);
	
	finalColor = tex * fragColor;
}
