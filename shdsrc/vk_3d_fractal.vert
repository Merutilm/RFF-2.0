#version 450


layout (set = 0, binding = 0) uniform IterUBO {
    uvec2 extent;
    double max_value;
} iteration_info_settings;

layout (set = 0, binding = 1) buffer IterSSBO {
    double iterations[];
} iteration_settings;

layout (set = 1, binding = 0) buffer PaletteSSBO {
    uint size;
    float interval;
    double offset;
    uint smoothing;
    uint single_smoothing;
    float animation_speed;
    vec4 palette[];
} palette_settings;


layout (set = 2, binding = 0) uniform TimeUBO {
    float time;
} time_settings;


layout (set = 3, binding = 0) uniform CameraUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} camera_settings;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexcoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexcoord;

void main(){
    float x = float(gl_VertexIndex % width);
    float y = float(gl_VertexIndex / width);
    float z = float(iteration_settings.iterations[gl_VertexIndex]);

    gl_Position = camera_settings.proj * camera_settings.view * camera_settings.model * vec4(x, y, z, 1.0);
    fragColor = inColor;
    fragTexcoord = inTexcoord;
}