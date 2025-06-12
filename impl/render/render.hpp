#pragma once

class render
{
public:
    bool initialize();

    void text(int x, int y, ImColor color, const char* text, bool center = true, bool outline = true);
    void cornered_box(int x, int y, int w, int h, ImColor color, int thickness);
private:
    struct resources
    {
        HWND hwnd = nullptr;
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* device_context = nullptr;
        ID3D11RenderTargetView* render_target_view = nullptr;
        IDXGISwapChain* swap_chain = nullptr;
    } s_resources;

    void present_loop();

    void set_window_pos(int x, int y, int width, int height, UINT flags);

    bool setup_d3d11();
    bool hijack_window();
    void refresh_window();
} inline c_render;

