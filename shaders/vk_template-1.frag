#version 450



layout(set = 0, binding = 0) uniform sampler2D image;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;
layout (location = 0) out vec4 color;

void main(){
    color = texture(image, fragTexcoord);
}