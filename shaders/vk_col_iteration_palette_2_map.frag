#version 450
#define NONE 0
#define NORMAL 1
#define REVERSED 2


layout (set = 0, binding = 0) buffer Iteration2MapNormalSSBO {
    double iterations[];
} it2map_normal_attr;

layout (set = 0, binding = 1) buffer Iteration2MapZoomedSSBO {
    double iterations[];
} it2map_zoomed_attr;

layout (set = 0, binding = 2) uniform Iteration2MapUBO {
    uvec2 extent;
    double max_value;
    float time;
} it2map_attr;

layout (set = 1, binding = 0) uniform VideoUBO {
    float default_zoom_increment;
    float current_frame;
} video_attr;

layout (set = 2, binding = 0) buffer PaletteSSBO {
    uint size;
    float interval;
    double offset;
    uint smoothing;
    float animation_speed;
    vec4 palette[];
} palette_attr;


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

vec4 get_color(double iteration) {

    if (iteration == 0 || iteration >= it2map_attr.max_value) {
        return vec4(0, 0, 0, 1);
    }
    switch (palette_attr.smoothing) {
        case NONE:
            iteration = iteration - mod(iteration, 1);
            break;
        case NORMAL:
            break;
        case REVERSED:
            iteration = iteration + 1 - 2 * mod(iteration, 1);
            break;
    }


    double timed_offset_ratio = palette_attr.offset - double(it2map_attr.time) * palette_attr.animation_speed / palette_attr.interval;
    double palette_offset_ratio = mod(iteration / palette_attr.interval + timed_offset_ratio, 1);
    double palette_offset = palette_offset_ratio * palette_attr.size;
    float palette_offset_decimal = float(mod(palette_offset, 1));

    uint cpl = uint(palette_offset_ratio * palette_attr.size);
    uint npl = (cpl + 1) % palette_attr.size;

    vec4 cc = palette_attr.palette[cpl];
    vec4 nc = palette_attr.palette[npl];

    return cc * (1 - palette_offset_decimal) + nc * (palette_offset_decimal);
}

double get_iteration_normal(uvec2 iter_coord, uvec2 offset){
    iter_coord.y = it2map_attr.extent.y - iter_coord.y;
    iter_coord += offset;
    return it2map_normal_attr.iterations[iter_coord.y * it2map_attr.extent.x + iter_coord.x];
}


double get_iteration_zoomed(uvec2 iter_coord, uvec2 offset){
    iter_coord.y = it2map_attr.extent.y - iter_coord.y;
    iter_coord += offset;
    return it2map_zoomed_attr.iterations[iter_coord.y * it2map_attr.extent.x + iter_coord.x];
}

void main() {

    vec2 coord = gl_FragCoord.xy;
    vec2 center = vec2(it2map_attr.extent) / 2.0;
    coord -= center;
    float r = int(max(0, video_attr.current_frame)) - video_attr.current_frame;

    float nsr = pow(video_attr.default_zoom_increment, r + 1);// r = 0 ~ 1
    float zsr = pow(video_attr.default_zoom_increment, r);// r = -1 ~ 0

    vec2 ntx = coord / nsr + center;
    vec2 ztx = coord / zsr + center;

    vec2 nrt = mod(ntx, 1);
    vec2 zrt = mod(ztx, 1);

    double nMain = get_iteration_normal(uvec2(ntx), uvec2(0, 0));
    double zMain = get_iteration_zoomed(uvec2(ztx), uvec2(0, 0));

    vec4 nc1 = get_color(nMain);
    vec4 nc2 = get_color(get_iteration_normal(uvec2(ntx), uvec2(1, 0)));
    vec4 nc3 = get_color(get_iteration_normal(uvec2(ntx), uvec2(0, 1)));
    vec4 nc4 = get_color(get_iteration_normal(uvec2(ntx), uvec2(1, 1)));

    vec4 nc5 = nc1 - (nc1 - nc2) * nrt.x;
    vec4 nc6 = nc3 - (nc3 - nc4) * nrt.x;
    vec4 nc7 = nc5 - (nc5 - nc6) * nrt.y;

    if (ztx.x >= it2map_attr.extent.x - 1 || ztx.y >= it2map_attr.extent.y - 1 || ztx.x < 0 || ztx.y < 0 || zMain == 0) {
        color = nc7;
    } else {
        vec4 zc1 = get_color(zMain);
        vec4 zc2 = get_color(get_iteration_zoomed(uvec2(ztx), uvec2(1, 0)));
        vec4 zc3 = get_color(get_iteration_zoomed(uvec2(ztx), uvec2(0, 1)));
        vec4 zc4 = get_color(get_iteration_zoomed(uvec2(ztx), uvec2(1, 1)));

        vec4 zc5 = zc1 - (zc1 - zc2) * zrt.x;
        vec4 zc6 = zc3 - (zc3 - zc4) * zrt.x;
        vec4 zc7 = zc5 - (zc5 - zc6) * zrt.y;

        color = nc7 * (-r) + zc7 * (r + 1);
    }

}
