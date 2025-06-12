#pragma once

class visuals
{
private:
	void render_box(int x, int y, int w, int h, ImColor color, ImDrawList* drawlist );
	void render_skeleton(void* player, ImColor color, ImDrawList* drawlist );
public:
	void render();
}inline g_visuals;