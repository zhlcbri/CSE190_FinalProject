#pragma once

#ifndef MANDELBROT_H_I0UEF3KR
#define MANDELBROT_H_I0UEF3KR

#include "rpc/msgpack.hpp"
#include <vector>

struct pixel {
    double x, y, z;
    MSGPACK_DEFINE_ARRAY(x, y, z)
};

using pixel_data = std::vector<pixel>;

#endif /* end of include guard: MANDELBROT_H_I0UEF3KR */

