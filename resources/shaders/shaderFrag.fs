#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;

const float RENDER_WIDTH = 400.0;
const float RENDER_HEIGHT = 200.0;

vec2 pixelizeTexCoord(vec2 texCoord)
{
    vec2 renderResolution = vec2(RENDER_WIDTH, RENDER_HEIGHT);
    return (floor(texCoord * renderResolution) + 0.5) / renderResolution;
}



void main()
{
    
	
    
    
    
    
    
    
    // normal rendering
    //vec2 pixelTexCoord = pixelizeTexCoord(fragTexCoord);
	//vec4 tex = texture(texture0, pixelTexCoord);    
    //vec4 color = tex * fragColor;


    


    // chromatic aberation
    vec2 center = vec2(0.5, 0.5);

    float chromaFactor = ((length(fragTexCoord - center)) * 0.05);
    // chromaFactor = 1 - pow(1 - pow(chromaFactor, 4), 4);
    chromaFactor = 1 - chromaFactor;

    vec2 pixelTexCoord = pixelizeTexCoord(fragTexCoord);
    vec2 rTexCoord = (fragTexCoord - center) * chromaFactor * 0.98 + center;
    vec2 gTexCoord = (fragTexCoord - center) * chromaFactor + center;
    vec2 bTexCoord = (fragTexCoord - center) * chromaFactor * 0.99 +  center;


    vec4 rTex = texture(texture0, pixelizeTexCoord(rTexCoord));
    vec4 bTex = texture(texture0, pixelizeTexCoord(gTexCoord));
    vec4 gTex = texture(texture0, pixelizeTexCoord(bTexCoord));

    vec4 color = vec4(rTex.r, gTex.g, bTex.b, 1.0) * fragColor;

    

    // crt
    vec2 coord = pixelizeTexCoord(fragTexCoord);
    float crtValue = 1 - (abs(sin(coord.y * 200.0)) * 0.1);

    color.r *= crtValue;
    color.g *= crtValue;
    color.b *= crtValue;


    // black borders
    float blackness = 1 - length(center - pixelizeTexCoord(fragTexCoord));

    color.r *= blackness;
    color.g *= blackness;
    color.b *= blackness;
    



    // color tint
    
    // green 25 147 82 vec4(0.10, 0.58, 0.32, 1.0)
    // brown vec4(0.44, 0.26, 0.08, 1.0)
    // bright red vec4(1.0, 0.0, 0.0, 1.0)
    // black ? vec(0.0, 0.0, 0.0, 1.0)

    const vec4 tint = vec4(0.0, 0.0, 0.0, 1.0);
    const float tintStrength = 0.05;
    color = vec4(
        mix(color.r, tint.r, tintStrength),
        mix(color.g, tint.g, tintStrength),
        mix(color.b, tint.b, tintStrength),
        1.0
    );



	finalColor = color;
}
