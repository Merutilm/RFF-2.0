#version 450

uniform sampler2D inputTex;
uniform ivec2 resolution;

in vec4 fColor;
out vec4 color;


void main() {

    vec2 coord = gl_FragCoord.xy / resolution;

    float x = coord.x;
    float y = coord.y;

    if (x < 0 || y < 0){
        discard;
    }
    if (x >= 1 || y >= 1){
        discard;
    }

    color = texture(inputTex, coord);
}
