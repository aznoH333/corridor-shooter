#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;

const float RENDER_WIDTH = 600.0;
const float RENDER_HEIGHT = 400.0;

vec2 pixelizeTexCoord(vec2 texCoord)
{
    vec2 renderResolution = vec2(RENDER_WIDTH, RENDER_HEIGHT);
    return (floor(texCoord * renderResolution) + 0.5) / renderResolution;
}


float adjustForContrast(float value) {
    // base version
    //return 1 - pow(1 - pow(value, 2), 2);

    //return 1 - pow(1 - pow(value, 4), 4);


    // test
    
    return 
    max(
        1 - pow(1 - pow(value, 4), 4),
        value
    );

}


void main()
{
    
	
    vec2 pixelTexCoord = pixelizeTexCoord(fragTexCoord);
	vec4 tex = texture(texture0, pixelTexCoord);
    vec4 color = tex * fragColor;


    // increase contrast
    // this exaggerates the bright colors and makes dark colors darker
    float colorValue = (color.r + color.g + color.b) / 3;
    float adjustedR = adjustForContrast(color.r);
    float adjustedG = adjustForContrast(color.g);
    float adjustedB = adjustForContrast(color.b);
    color = vec4(adjustedR, adjustedB, adjustedG, 1.0);



    // color tint
    const vec4 tint = vec4(0.44, 0.26, 0.08, 1.0);
    const float tintStrength = 0.05;
    color = vec4(
        mix(color.r, tint.r, tintStrength),
        mix(color.g, tint.g, tintStrength),
        mix(color.g, tint.b, tintStrength),
        1.0
    );



	finalColor = color;
}
