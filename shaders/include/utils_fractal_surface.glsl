#include <desc_fractal_surface.glsl>
#include <desc_iteration.glsl>
#include <desc_time.glsl>
#include <utils_iteration.glsl>

#ifndef UTILS_FRACTAL_SURFACE_INCLUDE
#define UTILS_FRACTAL_SURFACE_INCLUDE


vec3 get_normal(
    ivec2 iter_coord, float m
) {
    double ld = get_iteration(iter_coord, ivec2(-1, -1));
    double d = get_iteration(iter_coord, ivec2(0, -1));
    double rd = get_iteration(iter_coord, ivec2(1, -1));
    double l = get_iteration(iter_coord, ivec2(-1, 0));
    double r = get_iteration(iter_coord, ivec2(1, 0));
    double lu = get_iteration(iter_coord, ivec2(-1, 1));
    double u = get_iteration(iter_coord, ivec2(0, 1));
    double ru = get_iteration(iter_coord, ivec2(1, 1));
    float multiplier = float(iteration_info_settings.extent.x) / 1280.0 * m;

    float dzDx = float((rd + 2.0 * r + ru) - (ld + 2.0 * l + lu)) * multiplier;
    float dzDy = float((lu + 2.0 * u + ru) - (ld + 2.0 * d + rd)) * multiplier;

    vec2 uv = vec2(iter_coord) / vec2(iteration_info_settings.extent) + surface_settings.wave_offset;
    float t = float(time_settings.time);
    float wave_frequency = surface_settings.wave_frequency;
    float wave_speed = surface_settings.wave_speed;
    float wave_strength = surface_settings.wave_strength;
    float w1 = sin(dot(wave_frequency * uv, vec2(31.7, 12.3)) + sin(t * 1.7) * wave_speed * 1.11);
    float w2 = cos(dot(wave_frequency * uv, vec2(-18.4, 27.1)) + cos(t * 1.1) * wave_speed * 1.64);
    float w3 = sin(dot(wave_frequency * uv, vec2(42.1, -9.7)) + sin(t * 0.8) * wave_speed * 1.04);
    float w4 = cos(dot(wave_frequency * uv, vec2(-11.8, -35.2)) + cos(t * 1.4) * wave_speed * 2.11);

    float waveX = w1 * 0.12524 * sin(t * 1.05999) + w2 * 0.29965 * cos(t * 1.45154) + w3 * 0.35184 * sin(t * 1.73279) + w4 * 0.20245 * cos(t * 1.00153);
    float waveY = w1 * 0.13623 * cos(t * 1.60687) + w2 * 0.40775 * sin(t * 1.22541) + w3 * 0.22831 * cos(t * 1.32125) + w4 * 0.38924 * sin(t * 1.10382);

    waveX *= wave_strength;
    waveY *= wave_strength;


    return normalize(vec3(-dzDx + waveX, -dzDy + waveY, 1.0));
}

float slope_get_shade(ivec2 iter_coord) {
    if (surface_settings.shadow_brightness >= 1 || surface_settings.depth == 0) {
        return 1;
    }

    vec3 normal = get_normal(iter_coord, surface_settings.depth);
    float slope = acos(normal.z);
    float aspect = atan(normal.y, normal.x);

    float aRad = radians(surface_settings.light_azimuth);
    float zRad = radians(surface_settings.light_zenith);
    float shade = max(surface_settings.shadow_brightness, sin(zRad) * cos(slope) + cos(zRad) * sin(slope) * cos(aRad - aspect));
    return 1 - surface_settings.shadow_opacity * (1 - shade);
}


vec3 specular(ivec2 iter_coord){
    vec3 normal = get_normal(iter_coord, surface_settings.depth);
    vec3 perspective = vec3(0.0, 0.0, -1.0);
    vec3 reflection_direction = reflect(perspective, normal);
    vec3 light = normalize(
        vec3(
        cos(radians(surface_settings.light_azimuth)) * cos(radians(surface_settings.light_zenith)),
        sin(radians(surface_settings.light_azimuth)) * cos(radians(surface_settings.light_zenith)),
        sin(radians(surface_settings.light_zenith)))
    );
    float specular = pow(max(dot(reflection_direction, light), 0.0), surface_settings.light_sharpness) * surface_settings.light_strength;
    return vec3(specular);
}

vec3 get_perspective(){
    return vec3(0.0, 0.0, -1.0);
}

void reflection_refraction_coord(vec3 normal, ivec2 iter_coord, out ivec2 reflection_coord, out ivec2 refraction_coord){

    vec3 perspective = get_perspective();
    vec3 reflection_direction = reflect(perspective, normal);
    vec3 refraction_direction = refract(perspective, normal, 1.0 / surface_settings.refraction_ratio);

    vec2 reflection_offset = reflection_direction.xy / max(abs(reflection_direction.z), 0.001);
    vec2 refraction_offset = refraction_direction.xy / max(abs(refraction_direction.z), 0.001);
    reflection_offset = clamp(reflection_offset, vec2(-1.0), vec2(1.0)) * vec2(iteration_info_settings.extent);
    refraction_offset = clamp(refraction_offset, vec2(-1.0), vec2(1.0)) * vec2(iteration_info_settings.extent);


    float distortion = surface_settings.distortion_strength;
    reflection_coord = iter_coord + ivec2(reflection_offset * distortion);
    refraction_coord = iter_coord + ivec2(refraction_offset * distortion);
    reflection_coord = clamp(reflection_coord, ivec2(0), ivec2(iteration_info_settings.extent) - ivec2(1));
    refraction_coord = clamp(refraction_coord, ivec2(0), ivec2(iteration_info_settings.extent) - ivec2(1));
}


vec3 surface_get_shade(sampler2D canvas, ivec2 iter_coord)
{

    vec3 normal = get_normal(iter_coord, surface_settings.depth);
    ivec2 reflection_coord;
    ivec2 refraction_coord;

    reflection_refraction_coord(normal, iter_coord, reflection_coord, refraction_coord);

    vec3 reflection = texelFetch(canvas, reflection_coord, 0).rgb * slope_get_shade(reflection_coord) + specular(reflection_coord);
    vec3 refraction = texelFetch(canvas, refraction_coord, 0).rgb * slope_get_shade(refraction_coord) + specular(refraction_coord);
    float cosTheta = max(dot(-get_perspective(), normal), 0.0);
    float reflection_ratio = surface_settings.reflection_ratio;
    float fresnel = reflection_ratio + (1.0 - reflection_ratio) * pow(1.0 - cosTheta, 5.0);

    vec3 color = mix(
        refraction,
        reflection,
        fresnel
    );

    return color;
}


#endif