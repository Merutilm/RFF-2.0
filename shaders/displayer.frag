#version 450

uniform sampler2D inputTex;
uniform ivec2 resolution;

in vec4 fColor;
out vec4 color;


void main() {
    vec2 coord = gl_FragCoord.xy / resolution;
    color = texture(inputTex, coord);
}
