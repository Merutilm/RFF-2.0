#include <desc_render_meta.glsl>
#include <utils_complex.glsl>

#ifndef UTILS_MPA_INCLUDE
#define UTILS_MPA_INCLUDE


PA invalid_sentinel() {
    return PA(0, vec2(0), vec2(0), 0, 0);
}

bool is_valid(PA pa, float r) {
    return r < pa.radius;
}


PA lookup(uint64_t ref_iteration, vec2 dz) {
    if (ref_iteration == 0) {
        return invalid_sentinel();
    }

    MPIndexMapper mapper = mp_mapper_meta.mapper[uint(ref_iteration)];

    if (mapper.mapped == UINT64_MAX) {
        return invalid_sentinel();
    }

    const float r = norm_approx(dz);

    switch (mp_table_meta.selection_method) {
        case MSM_LOWEST:
            {
                PA pa = invalid_sentinel();

                for (int i = 0; i < mapper.levels; ++i) {
                    PA test = mp_table_meta.table[int(mapper.mapped) + i];
                    if (is_valid(test, r)) {
                        pa = test;
                    } else
                    return pa;
                }
                return pa;
                break;
            }
        case MSM_HIGHEST:
            {
                PA pa = mp_table_meta.table[int(mapper.mapped)];

                if (!is_valid(pa, r)) {
                    return invalid_sentinel();
                }

                for (int j = int(mapper.levels); j > 1; --j) {
                    PA test = mp_table_meta.table[int(mapper.mapped) + j - 1];

                    if (is_valid(test, r)) {
                        return test;
                    }
                }

                return pa;
                break;
            }
    }
    return invalid_sentinel();
}

vec2 apply(PA pa, vec2 dz, vec2 dc) {
    return mul_comp(pa.an, dz) + mul_comp(pa.bn, dc);
}

#endif