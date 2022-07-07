#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 uv;
in vec3 normal;
in vec4 worldPixel;

uniform sampler2D heightmap;
uniform sampler2D normalMap;

uniform sampler2D dirt, sand, grass, rock, snow;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

//inherit skybox bottom
uniform vec3 skyBottom;

vec3 lerp(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
}

void main() {
	vec3 normalColor = texture(normalMap, uv).rbg * 2 - 1;
	normalColor.b = -normalColor.b;
	normalColor.r = -normalColor.r;

	vec3 lightDir = normalize(lightDirection);
	float light = max(dot(-lightDir, normalColor), .25);

	vec3 dirtColor = texture(dirt, uv).rgb;
	vec3 sandColor = texture(sand, uv).rgb;
	vec3 grassColor = texture(grass, uv).rgb;
	vec3 rockColor = texture(rock, uv).rgb;
	vec3 snowColor = texture(snow, uv).rgb;

	float ds = clamp((worldPixel.y - 25) / 10, 0, 1);
	float sg = clamp((worldPixel.y - 50) / 10, 0, 1);
	float gr = clamp((worldPixel.y - 75) / 10, 0, 1);
	float rs = clamp((worldPixel.y - 100) / 10, 0, 1);

	vec3 diffuse = lerp(lerp(lerp(lerp(dirtColor, sandColor, ds), grassColor, sg), rockColor, gr), snowColor, rs);

	float d = distance(worldPixel.xyz, cameraPosition);
	float fogAmount = clamp((d - 50) / 250, 0, 1);		

	//vec3 viewDirection = normalize(worldPixel.xyz - cameraPosition);
	//vec3 lightReflect = normalize(reflect(-lightDir, normalColor));
	//float specular = pow(max(dot(lightReflect, viewDirection), 0.0), 128);

	FragColor = vec4(lerp(diffuse * light, skyBottom, fogAmount), 1.0);// + specular;
}