#include "rng.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define RNG_SEED_MULTIPLIER (1812433253ul)
#define RNG_GENERATE_MULTIPLIER (1181783497276652981ull)
#define RNG_FLOAT_MULTIPLIER (1.0f / 4294967296.0f)
#define RNG_RAND_MAX (0x7fff)

typedef struct Math_RNG_State {
    UInt64 _initial_seed;
    UInt64 _current_seed;

    UInt64 _values[16];
    UInt8  _index;
} RngState;

static RngState state;
static Bool     state_initialized = false;

void _math_rng_reset_state(UInt64 seed) {
    state._values[0] = seed;

    for (UInt8 i = 0; i < 16; ++i)
        state._values[i] = (seed ^ (seed >> 30)) + (VT_CAST(UInt64, i) * RNG_SEED_MULTIPLIER);

    state._index        = 0;
    state._current_seed = seed;
}
UInt64 _math_rng_generate_random(void) {
    UInt8        index_ = state._index;
    const UInt64 s_     = state._values[++index_];
    UInt64       t_     = state._values[index_ &= 15];

    t_ ^= t_ << 31;
    t_ ^= t_ >> 11;
    t_ ^= s_ ^ (s_ >> 30);

    state._values[index_] = t_;
    state._index          = index_;
    return (t_ * RNG_GENERATE_MULTIPLIER);
}

Bool math_rng_initialize(UInt64 seed) {
    if (state_initialized)
        return false;

    state._initial_seed = seed;
    _math_rng_reset_state(seed);

    return (state_initialized = true);
}
Bool math_rng_new_seed(UInt64 new_seed) {
    if ((new_seed == state._current_seed) || !state_initialized)
        return false;

    _math_rng_reset_state(new_seed);
    return true;
}
Bool math_rng_reset(void) {
    if (state._initial_seed == state._current_seed)
        return false;

    _math_rng_reset_state(state._initial_seed);
    return true;
}
UInt64 math_rng_get_initial_seed(void) { return state._initial_seed; }
UInt64 math_rng_get_current_seed(void) { return state._current_seed; }
Bool   math_rng_random_bool(void) { return VT_CAST(Bool, _math_rng_generate_random() & 1); }
Char   math_rng_random_char(Bool uppercase) { return (uppercase ? 'A' : 'a') + (VT_CAST(UInt32, _math_rng_generate_random()) % 26); }
Int32  math_rng_random_int(void) { return VT_CAST(Int32, _math_rng_generate_random() % INT32_MAX); }
Int32  math_rng_random_int_ranged(Int32 min, Int32 max) { return (max < min) ? ((VT_CAST(UInt32, _math_rng_generate_random()) % (min - max + 1)) + max) : ((VT_CAST(UInt32, _math_rng_generate_random()) % (max - min + 1)) + min); }
UInt32 math_rng_random_uint(void) { return VT_CAST(Int32, _math_rng_generate_random() % UINT32_MAX); }
UInt32 math_rng_random_uint_ranged(UInt32 min, UInt32 max) { return (max < min) ? ((VT_CAST(UInt32, _math_rng_generate_random()) % (min - max + 1)) + max) : ((VT_CAST(UInt32, _math_rng_generate_random()) % (max - min + 1)) + min); }
Flt32  math_rng_random_flt32(void) { return (VT_CAST(Flt32, VT_CAST(UInt32, _math_rng_generate_random())) * RNG_FLOAT_MULTIPLIER * RNG_RAND_MAX); }
Flt32  math_rng_random_flt32_ranged(Flt32 min, Flt32 max) {
    return (max < min) ? ((VT_CAST(Flt32, VT_CAST(UInt32, _math_rng_generate_random())) * RNG_FLOAT_MULTIPLIER) * ((min - max) + 1) + max) : ((VT_CAST(Flt32, VT_CAST(UInt32, _math_rng_generate_random())) * RNG_FLOAT_MULTIPLIER) * ((max - min) + 1) + min);
}
Flt64 math_rng_random_flt64(void) { return VT_CAST(Flt64, _math_rng_generate_random() * RNG_FLOAT_MULTIPLIER * RNG_RAND_MAX); }
Flt64 math_rng_random_flt64_ranged(Flt64 min, Flt64 max) {
    return (max < min) ? (VT_CAST(Flt64, _math_rng_generate_random() * RNG_FLOAT_MULTIPLIER * RNG_RAND_MAX) * ((min - max) + 1) + max) : (VT_CAST(Flt64, _math_rng_generate_random() * RNG_FLOAT_MULTIPLIER * RNG_RAND_MAX) * ((max - min) + 1) + min);
}
Flt32 math_rng_random_fraction(Int32 decimal_places) {
    if (decimal_places < 0)
        return 0.0f;

    Flt32 rand_ = math_rng_random_flt32();
    Flt32 mul_  = 1.0f;

    for (Int32 i = 0; i < decimal_places; ++i)
        mul_ *= 10.0f;

    return (rand_ / mul_);
}
Vec2 math_rng_random_vec2(void) { return VT_UNION(Vec2, math_rng_random_flt32(), math_rng_random_flt32()); }
Vec2 math_rng_random_vec2_ranged(Flt32 min, Flt32 max) { return VT_UNION(Vec2, math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max)); }
Vec3 math_rng_random_vec3(void) { return VT_UNION(Vec3, math_rng_random_flt32(), math_rng_random_flt32(), math_rng_random_flt32()); }
Vec3 math_rng_random_vec3_ranged(Flt32 min, Flt32 max) { return VT_UNION(Vec3, math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max)); }
Vec4 math_rng_random_vec4(Bool random_w) { return VT_UNION(Vec4, math_rng_random_flt32(), math_rng_random_flt32(), math_rng_random_flt32(), random_w ? math_rng_random_flt32() : 0.0f); }
Vec4 math_rng_random_vec4_ranged(Flt32 min, Flt32 max, Bool random_w) {
    return VT_UNION(Vec4, math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max), random_w ? math_rng_random_flt32_ranged(min, max) : 0.0f);
}
Mat4 math_rng_random_mat4(Bool random_w) { return VT_UNION(Mat4, math_rng_random_flt32(), math_rng_random_flt32(), math_rng_random_flt32(), random_w ? math_rng_random_flt32() : 0.0f); }
Mat4 math_rng_random_mat4_ranged(Flt32 min, Flt32 max, Bool random_w) {
    return VT_UNION(Mat4, math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max), random_w ? math_rng_random_flt32_ranged(min, max) : 0.0f);
}
Quat4 math_rng_random_quat4(Bool random_w) { return VT_UNION(Quat4, math_rng_random_flt32(), math_rng_random_flt32(), math_rng_random_flt32(), random_w ? math_rng_random_flt32() : 0.0f); }
Quat4 math_rng_random_quat4_ranged(Flt32 min, Flt32 max, Bool random_w) {
    return VT_UNION(Quat4, math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max), math_rng_random_flt32_ranged(min, max), random_w ? math_rng_random_flt32_ranged(min, max) : 0.0f);
}
