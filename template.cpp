/*
This is a template with all the boilerplate code already written.
There is nothing to change and the only thing to do to render is to modify 
the screen array inside of the while loop.
*/

#define UNICODE
#include <windows.h>
#include <stdio.h>
#include <chrono>

using namespace std;

int w = 120;
int h = 40;

int Pos(float x, float y){
    return w*(int)y+(int)x;
}

int main(){
    //This one dimensional array will be written on the console at every frame.
    wchar_t *screen = new wchar_t[w*h]; 
    
    //Fill it with spaces first to prevent weird shit from happening
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            screen[i*w+j] = ' ';
        }
    }

    //Set the screen up
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    while (1){
        tp2 = chrono::system_clock::now();
		chrono::duration<float> deltaTime = tp2 - tp1;
		tp1 = tp2;
        float dt = deltaTime.count();

        /* 
        render whatever you want here 
        */

        //use swprintf_s() to render permanent stuff like UI or info
        screen[w * h - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, h*w, {0, 0}, &dwBytesWritten);
    }

return 0;
}