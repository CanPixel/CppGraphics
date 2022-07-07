#version 330 core

out vec4 FragColor;

in vec4 worldPixel;

uniform vec3 cameraPosition;
uniform vec3 skyBottom;

vec3 lerp(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
}

void main() {
	vec3 lightDir = normalize(vec3(0, -0.5, -1));
	vec3 viewDirection = normalize(worldPixel.xyz - cameraPosition);

	vec3 top = vec3(0.1, 0.1, 0.2);
	//vec3 bot = vec3(0.2, 0.2, 0.4);

	float sun = pow(max(dot(-viewDirection, lightDir), 0.0), 256); 

	FragColor = vec4(lerp(skyBottom, top, viewDirection.y * 3.0) + sun * vec3(0.85, 0.55, 0.9), 1.0);

	// More stars
	for (int i = 0; i < 6; i++) {
		float j = i / 6.0;

		vec3 starDir = normalize(vec3(-j, -j, -j));
		float star = pow(max(dot(-viewDirection, starDir), 0.0), 16 * i);
		FragColor += vec4(star * vec3(0.2, 0.4, 0.15), 0.0);
	}
}