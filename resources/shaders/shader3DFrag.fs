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
	vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	float shininess = 1.0;

    
    vec4 spec = vec4(0.0);
    

    vec3 n = normalize(LightDataIn.normal);
	vec3 l = normalize(LightDataIn.lightDir);
	vec3 e = normalize(LightDataIn.eye);

	float intensity = max(dot(n,l), 0.0);
	if (intensity > 0.0) {
		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = specular * pow(intSpec, shininess);
	}

	vec4 light = max(intensity * diffuse + spec, ambient);


	
	vec4 tex = texture(texture0, fragTexCoord);
    
	
	finalColor = tex * fragColor * light;//vec4(vLighting, 1.0);
}

