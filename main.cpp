#include "general.hpp"

int main()
{
	LI_FN(MessageBoxA)(nullptr, oxorany("Please Press OK in lobby"), oxorany("0xD"), MB_OK | MB_ICONINFORMATION);

	while (!g_kernel.get_process_id(oxorany(L"FortniteClient-Win64-Shipping.exe")))
	{
		LI_FN(MessageBoxA)(nullptr, oxorany("Please Press OK in lobby"), oxorany("0xD"), MB_OK | MB_ICONINFORMATION);
	}

	if (!g_kernel.initialize())
	{
		LI_FN(MessageBoxA)(nullptr, oxorany("Failed To Initialize Memory"), oxorany("0x35B"), MB_OK | MB_ICONINFORMATION);
	}

	if (!g_kernel.attach(oxorany(L"FortniteClient-Win64-Shipping.exe")))
	{
		LI_FN(MessageBoxA)(nullptr, oxorany("Failed To Attach"), oxorany("0x92H"), MB_OK | MB_ICONINFORMATION);
	}

	if (std::filesystem::exists(oxorany("C:\\astrid.json")))
	{
		settings::load_config(oxorany("C:\\astrid.json"));
	}

	std::thread([&]() { g_updater.go(); }).detach();

	if (!g_render.initialize())
	{
		LI_FN(MessageBoxA)(nullptr, oxorany("Failed To Render"), oxorany("0x92H"), MB_OK | MB_ICONINFORMATION);
	}

	return LI_FN(MessageBoxA)(nullptr, oxorany("Closing..."), oxorany("0x92H"), MB_OK | MB_ICONINFORMATION);;
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID)
{
	if (reason != DLL_PROCESS_ATTACH)
		return true;

	std::thread(main).detach();
}


//extern "C" std::size_t DllMain_cpp(HMODULE module, DWORD reason, LPVOID) noexcept
//{
//	if (reason != DLL_PROCESS_ATTACH)
//		return true;
//
//	auto _CreateThread = LI_FN(CreateThread).forwarded_safe_cached();
//
//	_CreateThread(nullptr, 0, main, nullptr, 0, nullptr);
//}