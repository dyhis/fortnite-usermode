#include "general.hpp"

bool render::initialize()
{
	if (!this->hijack_window())
	{
		LI_FN(MessageBoxA)(nullptr, oxorany("Failed To Setup Window"), oxorany("0x92H"), MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if (!this->setup_d3d11())
	{
		LI_FN(MessageBoxA)(nullptr, oxorany("Failed To Setup d3d11"), oxorany("0x92H"), MB_OK | MB_ICONINFORMATION);
		return false;
	}

	this->present_loop();
}

bool render::hijack_window()
{
	s_resources.hwnd = FindWindowA("", nullptr); // no overlay 4 u boi
	if (!s_resources.hwnd) return false;

	return true;
}

bool render::setup_d3d11()
{
	DXGI_SWAP_CHAIN_DESC sc_desc{};
	sc_desc.BufferCount = 2;
	sc_desc.BufferDesc.Width = 0;
	sc_desc.BufferDesc.Height = 0;
	sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_desc.BufferDesc.RefreshRate.Numerator = 60;
	sc_desc.BufferDesc.RefreshRate.Denominator = 1;
	sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.OutputWindow = s_resources.hwnd;
	sc_desc.SampleDesc.Count = 1;
	sc_desc.SampleDesc.Quality = 0;
	sc_desc.Windowed = TRUE;
	sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL selected_feature_level;

	auto result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		feature_levels,
		_countof(feature_levels),
		D3D11_SDK_VERSION,
		&sc_desc,
		&s_resources.swap_chain,
		&s_resources.device,
		&selected_feature_level,
		&s_resources.device_context
	);

	if (FAILED(result))
	{
		return false;
	}

	ID3D11Texture2D* back_buffer = nullptr;
	result = this->s_resources.swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (FAILED(result) || back_buffer == nullptr)
	{
		return false;
	}

	result = this->s_resources.device->CreateRenderTargetView(back_buffer, nullptr, &this->s_resources.render_target_view);
	back_buffer->Release();

	if (FAILED(result))
	{
		return false;
	}

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(this->s_resources.hwnd);
	ImGui_ImplDX11_Init(this->s_resources.device, this->s_resources.device_context);

	auto& io = ImGui::GetIO();
	{
		io.IniFilename = "";
		io.Fonts->AddFontFromMemoryTTF(poppins_medium, sizeof(poppins_medium), 17.f);
	}

	return true;
}

void render::present_loop()
{
	MSG msg = { NULL };
	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessageA(&msg, this->s_resources.hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;

		POINT p_cursor;
		GetCursorPos(&p_cursor);
		io.MousePos.x = p_cursor.x;
		io.MousePos.y = p_cursor.y;

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		RECT rect;
		if (GetClientRect(s_resources.hwnd, &rect))
		{
			if ((rect.right - rect.left) != globals::screen_width || rect.left != 0)
				this->set_window_pos(0, 0, globals::screen_width, globals::screen_height, SWP_NOZORDER);
		}

		if (GetAsyncKeyState(VK_END))
		{
			exit(0);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			if (GetAsyncKeyState(VK_INSERT) & 1)
				c_menu.rendering = !c_menu.rendering;

			//if (GetForegroundWindow() == FindWindowA( "UnrealWindow", nullptr ))

			g_visuals.render();
			c_menu.render();

			this->s_resources.swap_chain->Present(0, 0);
		}

		ImGui::Render();
		const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->s_resources.device_context->OMSetRenderTargets(1, &this->s_resources.render_target_view, nullptr);
		this->s_resources.device_context->ClearRenderTargetView(this->s_resources.render_target_view, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		this->s_resources.swap_chain->Present(0, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(this->s_resources.hwnd);
}

void render::set_window_pos(int x, int y, int width, int height, UINT flags)
{
	shadowsyscall<NTSTATUS>("NtUserSetWindowPos", s_resources.hwnd, 0, x, y, width, height, flags);
}

void render::text(int x, int y, ImColor color, const char* text, bool center, bool outlined)
{
	auto draw_list = ImGui::GetBackgroundDrawList();

	if (center)
	{
		auto text_size = ImGui::CalcTextSize(text);
		x = int(x - text_size.x / 2);
	}

	if (outlined)
	{
		draw_list->AddText(ImVec2(x - 1, y), ImColor(0, 0, 0), text);
		draw_list->AddText(ImVec2(x + 1, y), ImColor(0, 0, 0), text);
		draw_list->AddText(ImVec2(x, y - 1), ImColor(0, 0, 0), text);
		draw_list->AddText(ImVec2(x, y + 1), ImColor(0, 0, 0), text);
	}

	draw_list->AddText(ImVec2(x, y), color, text);
}

void render::cornered_box(int x, int y, int w, int h, ImColor color, int thickness)
{
	float line_w = (w / 1);
	float line_h = (h / 1);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + line_h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + line_w, y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - line_w, y), ImVec2(x + w, y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + line_h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - line_h), ImVec2(x, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + line_w, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - line_w, y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - line_h), ImVec2(x + w, y + h), color, thickness);
}