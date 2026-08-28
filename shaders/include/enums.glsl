#ifndef ENUMS_INCLUDE
#define ENUMS_INCLUDE

// Iteration Coloring Method
#define ICM_LINEAR 0
#define ICM_SQUARE_ROOT 1
#define ICM_LOG 2

// Single Iteration Coloring Method
#define SICM_NONE 0
#define SICM_NORMAL 1
#define SICM_REVERSED 2

// Stripe Type
#define ST_NONE 0
#define ST_SINGLE_DIRECTION 1
#define ST_SMOOTH 2
#define ST_SMOOTH_SQUARED 3

// MPA Selection Method
#define MSM_LOWEST 0
#define MSM_HIGHEST 1

// Decimalize Iteration Method
#define DIM_NONE 0
#define DIM_LINEAR 1
#define DIM_SQUARE_ROOT 2
#define DIM_LOG 3
#define DIM_LOGLOG 4

// Perturbation Main Iterator
#define PMI_CPU 0
#define PMI_GPU 1

#endif