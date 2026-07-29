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


// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 vLighting;


out LightData {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} LightDataOut;



vec3 calculateLighting() {
    vec3 ambient = vec3(0.3, 0.3, 0.3);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // Equivalent to reference directionalVector
    vec3 lightDirection = normalize(vec3(0.85, 0.8, 0.75));

    // Equivalent to reference: transformedNormal = uNormalMatrix * vec4(aVertexNormal, 1.0);
    // But since raylib gives mat3, do mat3 * vec3
    vec3 transformedNormal = (matNormal * vec4(vertexNormal, 0.0)).xyz;

    float directional = max(dot(transformedNormal, lightDirection), 0.0);
    return ambient + (lightColor * directional);
}


void applyPointLight() {
    vec4 pos = matModel * vec4(vertexPosition, 1.0);
    vec4 lPos = vec4(0.0, 2.0, 0.0, 1.0);

    LightDataOut.normal = normalize(matNormal * vec4(vertexNormal, 1.0)).xyz;
    LightDataOut.lightDir = vec3(lPos - pos);
    LightDataOut.eye = vec3(-pos);

}


void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;


    // calculate lighting
    vLighting = calculateLighting();


    applyPointLight();
    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
