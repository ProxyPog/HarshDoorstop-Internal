#pragma once

#include "../../includes.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class GUI {
public:
	static GUI& GetInstance() {
		static GUI instance;
		return instance;
	}

	Present oPresent = NULL;

	static HRESULT __stdcall hkPresentWrapper(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
		return GetInstance().hkPresent(pSwapChain, SyncInterval, Flags);
	}

	static HRESULT __stdcall WndProcWrapper(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return GetInstance().WndProc(hWnd, uMsg, wParam, lParam);
	}

private:

	HWND window = NULL;
	WNDPROC oWndProc = NULL;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	ID3D11RenderTargetView* mainRenderTargetView = NULL;
	bool HkPresentInitialized = false;

	HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void LoadFonts();

	void InitImGui();
	void RenderUI();
	void RenderMouse();

	GUI() = default;
	~GUI() = default;

	GUI(const GUI&) = delete;
	GUI& operator=(const GUI&) = delete;
};