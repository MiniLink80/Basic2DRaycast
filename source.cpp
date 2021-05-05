#define UNICODE
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include <chrono>

#define SPEED 3.5
#define ANGLE_SPEED 1.7
#define PI 3.141592f


using namespace std;

int w = 120;
int h = 40;

int map[16][16] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int Pos(float x, float y){
    return w*(int)y+(int)x;
}

float x = 13.68;
float y = 2.27;
float angle = -0.77;
float FOV = PI/2;


//Checks that there's no collision and moves
void ChangePos(float dx, float dy, wchar_t *scr, float dt){
    scr[Pos(x,y)] = ' ';

    if (!(x+dx < 0 || x+dx > w-1 || map[(int)y][(int)(x+dx)] == 1))
        x += dx * SPEED * dt;
    if (!(y+dy < 0 || y+dy > h || map[(int)(y+dy)][(int)x] == 1))
        y += dy * SPEED * dt;
    
}

int main(){
    //This one dimensional array will be written on the console at every frame.
    wchar_t *screen = new wchar_t[w*h]; 
    
    //we fill it with spaces to prevent weird shit from happening
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            screen[i*w+j] = ' ';
        }
    }

    screen[Pos(x, y)] = 'O';

    //boilerplate code to set the screen up
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

        //the past few lines are to calculate deltaTime. It's there to make movement consistent even when FPS changes.

        if(GetAsyncKeyState('A')){
            angle -= 1.0 * ANGLE_SPEED * dt;
        }
        if(GetAsyncKeyState('D')){
            angle += 1.0 * ANGLE_SPEED * dt;
        }
        if(GetAsyncKeyState('S')){
            ChangePos(-cosf(angle), -sinf(angle), screen, dt);
        }
        if(GetAsyncKeyState('W')){
            ChangePos(cosf(angle), sinf(angle), screen, dt);
        }
        if (GetAsyncKeyState(27)){ //escape
            break;
        }
        

        float middleRayX, middleRayY;
        //we cast 120 rays
        for (int a = 0; a < w; a++){
            float curA = (angle - FOV/2.0f) + ((float)a / (float)w) * FOV;
            
            bool hitWall = false;
            int stepCount = 0;
            float rayX = x;
            float rayY = y;
            while (!hitWall && stepCount < 1000){
                rayX += 0.025 * cosf(curA);
                rayY += 0.025 * sinf(curA);
                if (map[(int)rayY][(int)rayX] == 1)
                    hitWall = true;
                else
                    stepCount++;
            }
            float distance = sqrt((rayX-x)*(rayX-x)+(rayY-y)*(rayY-y));
            int size = (int)(h/distance)*1.25;
            if (a == 60){
                middleRayX = rayX;
                middleRayY = rayY;
            }
                //we fill everything if size is above 119 to prevent crashing
                if (size > 119){
                    for (int i = 0; i < h; i++){
                        screen[Pos(a, i)] = 0x2588;
                    }
                }
                else{
                    //we check if the ray is close to a cube edge and render the edge accordingly
                    if (fabsf(rayX - round(rayX)) > 0.05 || fabsf(rayY - round(rayY)) > 0.05){
                        for (int i = (int)(h-size)/2; i < (h+size)/2; i++){
                            int tile;
                            if (distance < 3.0)	    	tile = 0x2588;
                            else if (distance < 6.0)	tile = 0x2593;
                            else if (distance < 12.0)	tile = 0x2592;
                            else if (distance < 16.0)	tile = 0x2591;
                            screen[Pos(a, i)] = tile;
                        }
                    }

                    //empty space if we're close enough
                    else {
                        for (int i = (int)(h-size)/2; i < (h+size)/2; i++){
                            screen[Pos(a, i)] = ' ';
                        }
                    }

                    //fill top and bottom
                    for (int i = 0; i < (int)(h-size)/2; i++){
                        screen[Pos(a, i)] = ' ';
                    }
                    for (int i = (int)(h+size)/2; i < h; i++){
                        screen[Pos(a, i)] = ' ';
                    }
                }
        }   
        

        //the minimap is drawn
        for (int i = 0; i < 16; i++){
            for (int j  = 0; j < 16; j++){
                if (map[i][j] == 1)
                    screen[Pos(j, i)+w] = '#';
                else
                    screen[Pos(j, i)+w] = '.';
            }
        }
        swprintf_s(screen, 70, L"X=%.2f Y=%.2f Angle=%.2f, rX=%.2f rY=%.2f FPS=%.2f", x, y, angle, middleRayX, middleRayY, 1.0f/dt);
        screen[Pos(x,y)+w] = 'O';
        screen[w * h - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, h*w, {0, 0}, &dwBytesWritten);
    }
    
    
    return 0;
}