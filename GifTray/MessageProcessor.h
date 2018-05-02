#pragma once
class MessageProcessor
{
public:
	MessageProcessor();
	~MessageProcessor();

	LRESULT ProcessMessage(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
};


MessageProcessor::MessageProcessor()
{
}


MessageProcessor::~MessageProcessor()
{
}

inline LRESULT MessageProcessor::ProcessMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, message, wParam, lParam);;
}

