#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;


struct Light {
    vec3 position;
    float radius;
    vec3 color;
};

const int LIGHT_COUNT = 10;
uniform Light lights[LIGHT_COUNT];
uniform int usedLights;



uniform sampler2D texture0; // raylib default texture uniform name

out vec4 finalColor;


vec3 applyPointLight() {
    vec3 pos = fragPosition;


    vec3 outputColor = vec3(0, 0, 0);
    for (int i = 0; i < LIGHT_COUNT; ++i ) {

        if (i < usedLights) {
            Light light = lights[i];

            // calculate attenuation
            float distance = length(light.position - pos);
            float radius = light.radius;
            float fade = clamp(1.0 - distance / radius, 0.0, 1.0);
            float attenuation = fade * fade;

            outputColor += light.color * attenuation;
        }

    }


    return min(outputColor, vec3(1, 1, 1));

}




void main()
{

    // lighting new
   
	
	vec4 tex = texture(texture0, fragTexCoord);
    vec3 color = applyPointLight();
    

    finalColor = tex * fragColor * vec4(color, 1);

}
