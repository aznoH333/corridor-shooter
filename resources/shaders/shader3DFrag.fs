#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 vLighting;



in LightData {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} LightDataIn;


uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;





void main()
{
    

    vec4 diffuse = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 ambient = vec4(0.1, 0.0, 0.0, 1.0);
	vec4 specular = vec4(0.2, 0.2, 0.2, 1.0);
	float shininess = 1.0;

    
    vec4 spec = vec4(0.0);
    

    vec3 normalDirection = normalize(LightDataIn.normal);
	vec3 lightDirection = normalize(LightDataIn.lightDir);
	vec3 eyeDirection = normalize(LightDataIn.eye);



    float intensity = max( dot( normalDirection, lightDirection ), 0.0);
    // calculate specular
    if (intensity > 0.0) {
		vec3 h = normalize(lightDirection + eyeDirection );
		float intSpec = max(dot( h, normalDirection ), 0.0);
		spec = specular * pow(intSpec, shininess);
	}
    

    float distance = length(LightDataIn.lightDir);
    float radius = 100.0;

    float fade = clamp(1.0 - distance / radius, 0.0, 1.0);
    float attenuation = fade * fade;

    vec4 light = max(intensity * diffuse + spec, ambient) * attenuation;
    light.a = 1.0;



	
	vec4 tex = texture(texture0, fragTexCoord);
    

    finalColor = tex * fragColor * light;

}

