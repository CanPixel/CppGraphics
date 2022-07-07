#version 330 core
out vec4 FragColor;

in vec2 fragCoord;
uniform sampler2D _MainTex;

void main() {
	FragColor = texture(_MainTex, fragCoord);
}
