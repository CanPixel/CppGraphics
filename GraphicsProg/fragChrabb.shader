#version 330 core
out vec4 FragColor;

in vec2 fragCoord;
uniform sampler2D _MainTex;

void main() {
	float rFact = 0.9;
	float gFact = 0.95;
	float bFact = 1.0;

	vec2 d = fragCoord - vec2(0.5, 0.5);

	float r = texture(_MainTex, vec2(0.5, 0.5) + d * rFact).r;
	float g = texture(_MainTex, vec2(0.5, 0.5) + d * gFact).g;
	float b = texture(_MainTex, vec2(0.5, 0.5) + d * bFact).b;

	FragColor = vec4(r, g, b, 1.0);//texture(_MainTex, fragCoord);
}
