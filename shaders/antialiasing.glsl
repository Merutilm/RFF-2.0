#version 450

uniform sampler2D inputTex;
uniform bool use;

in vec4 fColor;
out vec4 color;

#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439
#define INV_PI 0.31830988618379067153776752674503

void main() {

    ivec2 coord = ivec2(gl_FragCoord.xy);

    if (!use) {
        color = texelFetch(inputTex, coord, 0);
        return;
    }
    vec4 c1 = texelFetch(inputTex, coord + ivec2(1, 0), 0);
    vec4 c2 = texelFetch(inputTex, coord + ivec2(0, 1), 0);
    vec4 c3 = texelFetch(inputTex, coord + ivec2(-1, 0), 0);
    vec4 c4 = texelFetch(inputTex, coord + ivec2(0, -1), 0);
    vec4 c5 = texelFetch(inputTex, coord + ivec2(1, 1), 0);
    vec4 c6 = texelFetch(inputTex, coord + ivec2(1, -1), 0);
    vec4 c7 = texelFetch(inputTex, coord + ivec2(-1, 1), 0);
    vec4 c8 = texelFetch(inputTex, coord + ivec2(-1, -1), 0);
    vec4 center = texelFetch(inputTex, coord, 0) * 9;
    vec4 near = (c1 + c2 + c3 + c4) * 3;
    vec4 diagonal = (c5 + c6 + c7 + c8);

    color = (center + near + diagonal) / 25;

}
