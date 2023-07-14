#include <iostream>
#include "cldraw.h"

using namespace std;

#define GRAY 128

const int CANVAS_W = WIDTH / 2;
const int CANVAS_H = HEIGHT - 2;

int palette[]{
    112,
    128,
    15,
    111,
    32,
    16,
    60,
    64,
};
const int palette_len = 8;

struct Brush {
    int x;
    int y;
    int color;
    int timer = 0;

    void control(Console& console) {
        if (timer > 0) {
            timer--;
            return;
        }

        if (console.is_pressed(VK_LEFT)) {
            if (x > 0) {
                x--;
                timer = 3;
            }
        }
        if (console.is_pressed(VK_RIGHT)) {
            if (x < CANVAS_W-1) {
                x++;
                timer = 3;
            }
        }
        if (console.is_pressed(VK_UP)) {
            if (y > 0) {
                y--;
                timer = 3;
            }
        }
        if (console.is_pressed(VK_DOWN)) {
            if (y < CANVAS_H - 1) {
                y++;
                timer = 3;
            }
        }
    }

    void controlColor(Console& console) {
        for (int i = 0; i < palette_len; i++) {
            if (console.is_pressed('1' + i)) {
                color = palette[i];
            }
        }
    }
};

class Canvas {
    int canvas[CANVAS_H][CANVAS_W]{};
public:
    void clear(int color) {
        for (int y = 0; y < CANVAS_H; y++) {
            for (int x = 0; x < CANVAS_W; x++) {
                canvas[y][x] = color;
            }
        }
    }

    void paint(Brush& brush) {
        int x = brush.x;
        int y = brush.y;
        if (x < 0 || y < 0 ||
            x >= CANVAS_W || y >= CANVAS_H) {
            return;
        }
        canvas[y][x] = brush.color;
    }

    void draw(Console& console) {
        for (int y = 0; y < CANVAS_H; y++) {
            for (int x = 0; x < CANVAS_W; x++) {
                console.pixel(x * 2, y, ' ',
                    canvas[y][x]);
                console.pixel(x * 2 + 1, y, ' ',
                    canvas[y][x]);
            }
        }
    }

    void drawBrush(Console& console, Brush& brush) {
        int color = canvas[brush.y][brush.x];
        console.pixel(brush.x * 2, brush.y, '<', color);
        console.pixel(brush.x * 2 + 1, brush.y, '>', color);
    }
};

int main() {
    Console console;
    console.create();

    Brush brush {0, 0, 112};
    Canvas canvas;
    canvas.clear(GRAY);
    while (true) {
        brush.control(console);
        brush.controlColor(console);
        if (console.is_pressed(VK_SPACE)) {
            canvas.paint(brush);
        }

        console.clear();
        canvas.draw(console);
        canvas.drawBrush(console, brush);
        // панель инструментов
        console.text("color:", 0, CANVAS_H+1);
        console.text("      ", 7, CANVAS_H + 1, brush.color);
        for (int i = 0; i < palette_len; i++) {
            console.pixel(25+i*2, CANVAS_H, '1'+i);
            console.pixel(25+i*2, CANVAS_H + 1, ' ', palette[i]);
        }

        console.flip(60);
    }
    return 0;
}