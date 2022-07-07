#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;
in vec4 worldPixel;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

//inherit skybox bottom
uniform vec3 skyBottom;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void clip(float r) {
    if (r <= 0.0) discard;
}

void main()
{
    vec3 normalColor = texture(texture_normal1, TexCoords).rbg * 2 - 1;
    normalColor.b = -normalColor.b;
    normalColor.r = -normalColor.r;

    vec3 lightDir = normalize(lightDirection);

    vec4 diffuse = texture(texture_diffuse1, TexCoords);
    vec4 specTex = texture(texture_specular1, TexCoords);

    float light = max(dot(-lightDir, normalColor), 0.0);

    //clip(light - 0.5);

    vec3 viewDir = normalize(cameraPosition - FragPos.rgb);
    vec3 refl = reflect(lightDir, Normals);

  //  float ambientOcclusion = 1;
 //   ambientOcclusion = texture(texture_ao1, TexCoords).r;

    float d = distance(FragPos.xyz, cameraPosition);
    float fogAmount = clamp((d - 50) / 250, 0, 1);

    float roughness = texture(texture_roughness1, TexCoords).r;
    float spec = pow(max(dot(viewDir, refl), 0.0), lerp(1, 256, roughness));
    vec3 specular = spec * specTex.rgb;

    vec4 result = diffuse /* max(light * ambientOcclusion, 0.2 * ambientOcclusion)*/ + vec4(specular, 0);
    result.a = diffuse.r;

    FragColor = result;
}