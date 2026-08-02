#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 vLighting;


in LightData {
	vec3 color;
} LightDataIn;




uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;





void main()
{

    // lighting new
   
	
	vec4 tex = texture(texture0, fragTexCoord);
    

    finalColor = tex * fragColor * vec4(LightDataIn.color, 1);

}

