#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 vLighting;



in LightData {
	vec3 lightDir;
} LightDataIn;


uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;





void main()
{

    // lighting new
    vec4 lightColor = vec4(1, 1, 1, 1);

    float distance = length(LightDataIn.lightDir);
    float radius = 30.0;
    float fade = clamp(1.0 - distance / radius, 0.0, 1.0);
    float attenuation = fade * fade;

    vec4 light = lightColor * attenuation;
    light.a = 1;
	
	vec4 tex = texture(texture0, fragTexCoord);
    

    finalColor = tex * fragColor * light;

}

