/*
This is a doom-like pseudo-3D rendering program. This code was inspired by javidx9's
first person shooter code. I borrowed the rendering logic (writing onto the console efficiently).
The rest of the logic is coded by yours truly. 
The way it works is that several rays are cast from the player (120 to be precise)
and each ray's intersection point with a wall is found. As soon as a ray hits a wall,
a vertical line is drawn onto the screen at the appropriate horizontal position. 
The line's length basically gives this illusion of 3d perspective.
The further away we are from the wall, the smaller that line will be. Since we know the 
distance between a wall and a player we can also shade it according to its distance
to give a better illusion.
Use WASD-QE to move around.
*/


#define UNICODE
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include <chrono>

#define SPEED 3
#define ANGLE_SPEED 1.5
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
    {1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

//Function to find the position with x and y coordinates
int Pos(float x, float y){
    return w*(int)y+(int)x;
}

//Be careful not to set the position inside of a wall. 
float x = 7.0;
float y = 7.0;
float angle = 0.0;
float FOV = PI/3;


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
    
    screen[Pos(x, y)] = 'O';

    //Boilerplate code to set the screen up
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
        //The past few lines are to calculate deltaTime. It's there to make movement consistent even when FPS changes.

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
        if(GetAsyncKeyState('E')){
            ChangePos(cosf(angle+PI/2)*0.6, sinf(angle+PI/2)*0.6, screen, dt);
        }
        if(GetAsyncKeyState('Q')){
            ChangePos(-cosf(angle+PI/2)*0.6, -sinf(angle+PI/2)*0.6, screen, dt);
        }
        if (GetAsyncKeyState(27)){ //escape
            break;
        }
        
        //120 rays are cast.
        for (int a = 0; a < w; a++){
            //This is the angle of the current ray.
            float curA = (angle - FOV/2.0f) + ((float)a / (float)w) * FOV;
            
            bool hitWall = false;
            int stepCount = 0;
            float rayX = x;
            float rayY = y;

            //The ray's position is incremented by a small step each time
            //to find the precise moment of contact with the wall.
            while (!hitWall && stepCount < 1000){
                
                rayX += 0.025 * cosf(curA);
                rayY += 0.025 * sinf(curA);
                
                if (map[(int)rayY][(int)rayX] == 1)
                    hitWall = true;
                else
                    stepCount++;
            }

            float distance = sqrt((rayX-x)*(rayX-x)+(rayY-y)*(rayY-y));
            int size = (int)(h/distance);
            
            //Fill everything if size is above 39 to prevent a segmentation fault.
            if (size > 39){
                for (int i = 0; i < h; i++){
                    screen[Pos(a, i)] = 0x2588;
                }
            }
            else{
                //Check if the ray is close to a cube edge and render the edge accordingly.
                if (fabsf(rayX - round(rayX)) > 0.05 || fabsf(rayY - round(rayY)) > 0.05){
                    for (int i = (int)(h-size)/2; i < (h+size)/2; i++){
                        //The wall is darker as we get further away from it.
                        int tile;
                        if (distance < 3.0)	        tile = 0x2588;
                        else if (distance < 6.0)	tile = 0x2593;
                        else if (distance < 12.0)	tile = 0x2592;
                        else                    	tile = 0x2591;
                        screen[Pos(a, i)] = tile;
                    }
                }

                //Empty space if the ray is close enough to render the edges as dark seams
                else {
                    for (int i = (int)(h-size)/2; i < (h+size)/2; i++){
                        screen[Pos(a, i)] = ' ';
                    }
                }

                //Fill ceiling and floor.
                for (int i = 0; i < (int)(h-size)/2; i++){
                    screen[Pos(a, i)] = ' ';
                }
                for (int i = (int)(h+size)/2; i < h; i++){
                    //Shade floor for next-gen graphics.
                    if (i < h-9)
                        screen[Pos(a, i)] = '.';
                    else if (i < h-4)
                        screen[Pos(a, i)] = 'x';
                    else
                        screen[Pos(a, i)] = '#';
                }
            }
        }   
        

        //The minimap is drawn.
        for (int i = 0; i < 16; i++){
            for (int j  = 0; j < 16; j++){
                if (map[i][j] == 1)
                    screen[Pos(j, i)+w] = '#';
                else
                    screen[Pos(j, i)+w] = ' ';
            }
        }
        
        swprintf_s(screen, 40, L"X=%.2f Y=%.2f Angle=%.2f, FPS=%.2f", x, y, angle, 1.0f/dt);
        screen[Pos(x,y)+w] = 'O';
        screen[w * h - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, h*w, {0, 0}, &dwBytesWritten);
    }
    
    return 0;
}
