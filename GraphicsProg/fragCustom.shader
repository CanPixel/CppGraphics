#version 330 core
out vec4 FragColor;

in vec2 fragCoord;
uniform sampler2D _MainTex;

uniform vec2 iResolution;

void main()
{
    vec2 q = fragCoord.xy;
    vec2 uv = 0.5 + (q + 0.5) * (0.9 + 0.1 * sin(0.2 * 100));

    vec3 oricol = texture(_MainTex, vec2(q.x, q.y)).xyz;
    vec3 col;

    col.r = texture(_MainTex, vec2(uv.x + 0.003, uv.y)).x;
    col.g = texture(_MainTex, vec2(uv.x + 0.000, uv.y)).y;
    col.b = texture(_MainTex, vec2(uv.x - 0.003, uv.y)).z;

    col = clamp(col * 0.5 + 0.5 * col * col * 1.2, 0.0, 1.0);

    col *= 0.5 + 0.5 * 16.0 * uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);

    col *= vec3(0.95, 1.05, 0.95);

    col *= 0.9 + 0.1 * sin(10.0 * 100 + uv.y * 1000.0);

    col *= 0.99 + 0.01 * sin(110.0 * 100);

    float comp = smoothstep(0.2, 0.7, sin(100));
    col = mix(col, oricol, clamp(-2.0 + 2.0 * q.x + 3.0 * comp, 0.0, 1.0));

    FragColor = vec4(col, 1.0);
}