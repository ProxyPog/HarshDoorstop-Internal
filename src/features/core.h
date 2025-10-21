#pragma once

class Core {
public:
	static Core& GetInstance() {
		static Core instance;
		return instance;
	}

	void Setup();
	void EnableHooks();
	bool InitVars();
	void HandleInputs();

private:
	Core() = default;
	~Core() = default;

	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;
};