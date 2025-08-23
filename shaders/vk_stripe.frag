#version 450

#define DOUBLE_PI 6.2831853071795864
#define NONE 0
#define SINGLE_DIRECTION 1
#define SMOOTH 2
#define SMOOTH_SQUARED 3

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput canvas;

layout (set = 1, binding = 0) buffer IterSSBO {
    uvec2 extent;
    double max_value;
    double iterations[];
} iteration_attr;

layout (set = 2, binding = 0) uniform StripeUBO {
    uint type;
    float firstInterval;
    float secondInterval;
    float opacity;
    float offset;
    float animationSpeed;
} stripe_attr;

layout (set = 3, binding = 0) uniform TimeUBO {
    float time;
} time_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;


double getIteration(uvec2 iterCoord){
    iterCoord.y = iteration_attr.extent.y - iterCoord.y;
    return iteration_attr.iterations[iterCoord.y * iteration_attr.extent.x + iterCoord.x];
}


void main() {

    uvec2 iterCoord = uvec2(gl_FragCoord.xy);
    double iteration = getIteration(iterCoord);

    if (stripe_attr.type == NONE || iteration == 0) {
        color = subpassLoad(canvas);
        return;
    }

    double iterCurr = iteration - (stripe_attr.offset + stripe_attr.animationSpeed * time_attr.time);
    float black;
    float rat1 = float(mod(iterCurr, stripe_attr.firstInterval)) / stripe_attr.firstInterval;
    float rat2 = float(mod(iterCurr, stripe_attr.secondInterval)) / stripe_attr.secondInterval;

    switch (stripe_attr.type) {
        case SINGLE_DIRECTION: {
                                   black = rat1 * rat2;
                                   break;
                               }
        case SMOOTH: {
                                   black = pow((sin(rat1 * DOUBLE_PI) + 1) * (sin(rat2 * DOUBLE_PI) + 1) / 4, 2);
                                   break;
                               }
        case SMOOTH_SQUARED: {
                                   black = pow((sin(rat1 * DOUBLE_PI) + 1) * (sin(rat2 * DOUBLE_PI) + 1) / 4, 4);
                                   break;
                               }
    }

    color = vec4((subpassLoad(canvas).rgb * (1 - black * stripe_attr.opacity)), 1);
}
