#version 450

uniform sampler2D inputTex;
uniform sampler2D blurred;
uniform ivec2 resolution;

uniform float softness;
uniform float intensity;

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

    vec3 blur = texture(blurred, coord).rgb;
    vec3 add = blur - (blur - color.rgb) * softness;
    color = color + vec4(add * intensity, 1);

}
