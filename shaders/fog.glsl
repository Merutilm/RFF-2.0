#version 450

uniform sampler2D inputTex;
uniform sampler2D blurred;
uniform ivec2 resolution;

uniform float opacity;

in vec4 fColor;
out vec4 color;

float grayScale(vec3 c){
    return c.r * 0.3 + c.g * 0.59 + c.b * 0.11;
}

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
    vec3 blurredColor = texture(blurred, coord).rgb;
    color = texture(inputTex, coord);

    vec3 cf = color.rgb - (color.rgb - blurredColor) * opacity;

    float cg = grayScale(color.rgb);
    float cfg = grayScale(cf);

    if(cg < cfg){
        color = vec4(cf, 1);
    }

}
