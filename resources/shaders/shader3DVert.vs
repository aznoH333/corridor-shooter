#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;


// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;
uniform mat4 matModel;


struct Light {
    vec3 position;
    float radius;
    vec3 color;
};

const int LIGHT_COUNT = 10;
uniform Light lights[LIGHT_COUNT];
uniform int usedLights;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;


out LightData {
    vec3 color;
} LightDataOut;


void applyPointLight() {
    vec3 pos = vec3(matModel * vec4(vertexPosition, 1.0));


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


    LightDataOut.color = min(outputColor, vec3(1, 1, 1));

}


void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;



    applyPointLight();
    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
