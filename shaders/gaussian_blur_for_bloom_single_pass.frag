#version 450

uniform sampler2D inputTex;
uniform sampler2D original;
uniform ivec2 resolution;

uniform float radius;
uniform float threshold;

in vec4 fColor;
out vec4 color;

float grayScale(vec3 c){
    return c.r * 0.3 + c.g * 0.59 + c.b * 0.11;
}

void main() {

    vec2 coord = gl_FragCoord.xy / resolution;

    float x = coord.x;
    float y = coord.y;
    float i = 1.0 / resolution.y;

    if(radius <= 0){
        color = texture(inputTex, coord);
        return;
    }

    if (x < 0 || y < 0){
        discard;
    }

    if (x >= 1 || y >= 1){
        discard;
    }

    if(grayScale(texture(original, coord).rgb) < threshold){
        color = vec4(0, 0, 0, 1);
        return;
    }

    vec3 sum = vec3(0, 0, 0);
    int count = 0;

    for (float x1 = max(0, x - radius); x1 <= min(1, x + radius); x1 += i){
        sum += texture(inputTex, vec2(x1, y)).rgb;
        count++;
    }
    for (float y1 = max(0, y - radius); y1 <= min(1, y + radius); y1 += i){
        sum += texture(inputTex, vec2(x, y1)).rgb;
        count++;
    }


    color = vec4(sum / count, 1);

}
