#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat3 matNormal;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 vLighting;




vec3 calculateLighting() {
    vec3 ambient = vec3(0.1, 0.1, 0.1);


    // spot lighting
    vec3 lightPosition = vec3(0.0, 0.5, 0.0); // TODO : set lights as uniforms
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 lightDirection = normalize(vertexPosition - lightPosition);

    /*
        highp vec4 transformedNormal = uNormalMatrix * vec4(aVertexNormal, 1.0);

      highp float directional = max(dot(transformedNormal.xyz, directionalVector), 0.0);
      vLighting = ambientLight + (directionalLightColor * directional);
    */

    vec4 transformedNormal = mvp * vec4(vertexNormal, 1.0);

    float directional = max(dot(transformedNormal.xyz, lightDirection), 0.0);



    return ambient + (lightColor * directional);
}



void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;


    // calculate lighting
    vLighting = calculateLighting();



    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
