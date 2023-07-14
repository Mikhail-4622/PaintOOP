#pragma once
#ifndef CLDRAW_H
#define CLDRAW_H

#include <windows.h> 
#include <stdio.h>

#define WIDTH 120
#define HEIGHT 30

constexpr auto CLDRAW_VERSION = 4;

int console_frame = 0;
HANDLE hStdout, hNewScreenBuffer, hNewScreenBuffer2;
SMALL_RECT srctWriteRect;
CHAR_INFO chiBuffer[WIDTH * HEIGHT];
COORD coordBufSize;
COORD coordBufCoord;
BOOL fSuccess;
HANDLE curScreenBufferHandle;
float console_timer = 0.0;

int create_console(const wchar_t title[] = L"CL-Draw Window v.1") {
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	coordBufSize.X = WIDTH;
	coordBufSize.Y = HEIGHT;

	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

	srctWriteRect.Left = srctWriteRect.Top = 0;
	srctWriteRect.Right = WIDTH - 1;
	srctWriteRect.Bottom = HEIGHT - 1;

	hNewScreenBuffer = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL);
	hNewScreenBuffer2 = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL);
	if (hNewScreenBuffer2 == INVALID_HANDLE_VALUE ||
		hNewScreenBuffer == INVALID_HANDLE_VALUE)
	{
		printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
		return 1;
	}

	SetConsoleTitle(title);
	SetConsoleScreenBufferSize(hNewScreenBuffer, coordBufSize);
	SetConsoleWindowInfo(hNewScreenBuffer, TRUE, &srctWriteRect);
	SetConsoleScreenBufferSize(hNewScreenBuffer2, coordBufSize);
	SetConsoleWindowInfo(hNewScreenBuffer2, TRUE, &srctWriteRect);

	return 0;
}

bool console_is_pressed(int key) {
	return GetAsyncKeyState(key) < 0;
}

void console_pixel(int x, int y, char c, int attrs = 0xF) {
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return;
	int index = (int)(x)+WIDTH * (int)(y);
	chiBuffer[index].Char.UnicodeChar = c;
	chiBuffer[index].Attributes = attrs;
}

void console_flip(int fps = 60) {
	if (console_frame % 2 == 0) {
		curScreenBufferHandle = hNewScreenBuffer;
	}
	else {
		curScreenBufferHandle = hNewScreenBuffer2;
	}

	fSuccess = WriteConsoleOutput(
		curScreenBufferHandle,
		chiBuffer,
		coordBufSize,
		coordBufCoord,
		&srctWriteRect);

	if (!SetConsoleActiveScreenBuffer(curScreenBufferHandle)) {
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		return;
	}

	Sleep((DWORD)(1 / (float)fps * 1000.0f));
	console_frame++;
	console_timer += 1.0f / (float)fps;
}

void console_clear(int attrs = 0) {
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			chiBuffer[x + WIDTH * y].Char.UnicodeChar = ' ';
			chiBuffer[x + WIDTH * y].Attributes = attrs;
		}
	}
}

void console_text(const char* text, int x, int y, int attrs = 0xF) {
	for (int i = 0; text[i]; i++) {
		char c = text[i];
		console_pixel(x + i, y, c, attrs);
	}
}

bool pressed[1000]{};
bool console_just_pressed(int code) {
	if (console_is_pressed(code)) {
		if (pressed[code]) {
			return false;
		}
		pressed[code] = true;
		return true;
	}
	else {
		pressed[code] = false;
		return false;
	}
}

class Console {
public:
	void create() {
		create_console();
	}

	void clear(int attrs = 0) {
		console_clear(attrs);
	}

	void pixel(int x, int y, char c, int attrs = 0xF) {
		console_pixel(x, y, c, attrs);
	}

	void flip(int fps = 60) {
		console_flip(fps);
	}

	void hline(int start_x, int start_y, int length,
		char c, int attrs = 0xF) {
		for (int x = start_x; x < start_x + length; x++) {
			pixel(x, start_y, c, attrs);
		}
	}

	void rectangle(int start_x, int start_y,
		int w, int h,
		char c, int attrs = 0xF) {
		for (int y = start_y; y < start_y + h; y++) {
			hline(start_x, y, w, c, attrs);
		}
	}

	bool is_pressed(int key) {
		return console_is_pressed(key);
	}

	bool just_pressed(int key) {
		return console_just_pressed(key);
	}

	void text(const char* text, int x, int y, int attrs = 0xF) {
		console_text(text, x, y, attrs);
	}
};

#endif // CLDRAW_H