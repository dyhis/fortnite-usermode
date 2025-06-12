#pragma once

static const char* keynames [ ] =
{

	"Keybind",
	"Left Mouse",
	"Right Mouse",
	"Cancel",
	"Middle Mouse",
	"Mouse 5",
	"Mouse 4",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
};

inline static bool items_array_getter ( void* data, int idx, const char** out_text )
{
	const char* const* items = ( const char* const* ) data;
	if (out_text)
		*out_text = items [ idx ];
	return true;
}

class hotkey
{
public:
	int& key;

private:
	bool waiting = false;
	HANDLE thread_handle = nullptr;

public:

	hotkey ( int& keybind ) : key ( keybind ) { };

	~hotkey ( )
	{
		if (thread_handle)
			CloseHandle ( thread_handle );
	}

	void add_button ( const ImVec2& size = ImVec2 ( 155, 40 ) )
	{
		const char* preview_value = nullptr;
		if (key >= 0 && key < IM_ARRAYSIZE ( keynames ))
			items_array_getter ( keynames, key, &preview_value );

		std::string button_text;
		if (waiting)
			button_text = "press the key";
		else if (preview_value)
			button_text = preview_value;
		else
			button_text = "select key";

		// Set dark grey style
		ImGui::PushStyleColor ( ImGuiCol_Button, ImColor ( 35, 35, 35, 255 ).Value );
		ImGui::PushStyleColor ( ImGuiCol_ButtonHovered, ImColor ( 45, 45, 45, 255 ).Value );
		ImGui::PushStyleColor ( ImGuiCol_ButtonActive, ImColor ( 25, 25, 25, 255 ).Value );

		if (ImGui::Button ( button_text.c_str ( ), size ))
		{
			if (!waiting)
			{
				waiting = true;
				thread_handle = CreateThread ( nullptr, 0, ( LPTHREAD_START_ROUTINE ) key_change_thread, this, 0, nullptr );
			}
		}

		ImGui::PopStyleColor ( 3 ); // Restore all 3 button states
	}

private:
	static DWORD WINAPI key_change_thread ( LPVOID param )
	{
		hotkey* self = reinterpret_cast< hotkey* >( param );
		while (self->waiting)
		{
			for (int i = 0; i < 0x87; ++i)
			{
				if (GetAsyncKeyState ( i ) & 0x8000)
				{
					self->key = i;
					self->waiting = false;
					return 0;
				}
			}
			Sleep ( 1 );
		}
		return 0;
	}
};



