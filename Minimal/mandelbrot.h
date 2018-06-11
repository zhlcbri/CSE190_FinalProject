#pragma once

#ifndef MANDELBROT_H_I0UEF3KR
#define MANDELBROT_H_I0UEF3KR

#include "rpc/msgpack.hpp"
#include <vector>

struct pixel {
    double r = 0, g = 0, b = 0;
    MSGPACK_DEFINE_ARRAY(r, g, b)
};

using pixel_data = std::vector<pixel>;

#endif /* end of include guard: MANDELBROT_H_I0UEF3KR */

