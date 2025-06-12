#include "general.hpp"

void input::click()
{
	g_kernel.inject_key(0x26, false);
	g_kernel.inject_key(0x26, true);
}

void input::move_to(int x, int y, float smoothing)
{
    const float screen_center_x = globals::screen_width_center;
    const float screen_center_y = globals::screen_height_center;

    float total_dx = static_cast<float>(x) - screen_center_x;
    float total_dy = static_cast<float>(y) - screen_center_y;

    if (total_dx == 0.f && total_dy == 0.f)
        return;

    if (total_dx != 0)
    {
        total_dx /= smoothing;
    }

    if (total_dy != 0)
    {
        total_dy /= smoothing;
    }
    
    g_kernel.inject_mouse(total_dx, total_dy);
}
