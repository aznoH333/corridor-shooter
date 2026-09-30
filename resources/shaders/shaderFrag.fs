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
        0 //value
    );

}


void main()
{
    
	
    vec2 pixelTexCoord = fragTexCoord;//pixelizeTexCoord(fragTexCoord);
	vec4 tex = texture(texture0, pixelTexCoord);
    vec4 color = tex * fragColor;


    // increase contrast
    // this exaggerates the bright colors and makes dark colors darker
    //float colorValue = max(color.r, max(color.b, color.g));//(color.r + color.g + color.b) / 3;
    //float adjustedR = adjustForContrast(color.r);
    //float adjustedG = adjustForContrast(color.g);
    //float adjustedB = adjustForContrast(color.b);
    //float contrastValue = adjustForContrast(colorValue);
    
    float smallestColorVal = min(color.r, min(color.g, color.b));
    float largestColorVal = max(color.r, max(color.g, color.b));
    
    // how close is the color to being "white" (dist of largest to smallest color)
    float whiteness = 1 - (largestColorVal - smallestColorVal);

    // adjust for brightness
    // float brightness = largestColorVal; 
    

    float poo = whiteness;
    color = vec4(
        poo, //max(adjustedR, color.r), 
        poo, //max(adjustedG, color.g), 
        poo, //max(adjustedB, color.b), 
    1.0);

    //color = vec4(contrastValue, contrastValue, contrastValue, 1.0);



    // color tint
 //   const vec4 tint = vec4(0.44, 0.26, 0.08, 1.0);
 //   const float tintStrength = 0.05;
 //   color = vec4(
 //       mix(color.r, tint.r, tintStrength),
 //       mix(color.g, tint.g, tintStrength),
 //       mix(color.g, tint.b, tintStrength),
 //       1.0
 //   );



	finalColor = color;
}
