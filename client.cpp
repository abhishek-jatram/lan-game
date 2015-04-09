#include <SDL.h>
#include <GL/gl.h>
#include <iostream>
#include <math.h>

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

struct position
{
    float x,y;
};
float width = 10.0f;
float speed = 10.0f;
position mypos,otherpos;

int clientSocket;
struct sockaddr_in server_addr;
struct hostent *ptrh;
bool isServerReady=false;
bool cc(float ax,float ay,float aw,float ah,float bx,float by,float bw,float bh)
{
    if(ay+ah<by)
        return false;
    else if(by+bh<ay)
        return false;
    else if(ax+aw<bx)
        return false;
    else if(bx+bw<ax)
        return false;
    else
        return true;
}

void handleKeys()
{
    Uint8* state=SDL_GetKeyState(NULL);
    if(state[SDLK_UP])
        mypos.y+=speed;
    if(state[SDLK_DOWN])
        mypos.y-=speed;
    if(state[SDLK_LEFT])
        mypos.x-=speed;
    if(state[SDLK_RIGHT])
        mypos.x+=speed;
}

void networkinit();
void init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_WM_SetCaption("SIMPLE NETWORK GAME",NULL);
    SDL_SetVideoMode(1200,700,32,SDL_OPENGL);
    glClearColor(0,0,0,1);
    glViewport(0,0,1200,700);
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glDisable(GL_DEPTH_TEST);
    mypos.x = 20.0;
	mypos.y = 20.0;
	otherpos.x = 1180.0;
	otherpos.y = 680.0;
	networkinit();
}
void networkinit(){
    clientSocket=socket(AF_INET,SOCK_STREAM,0);
    memset((char*)&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10000);

    ptrh=gethostbyname("10.9.20.42");//ip address of server and have same mask on both devices
    memcpy(&server_addr.sin_addr,ptrh->h_addr,ptrh->h_length);
    std::cout<<"Waiting for server..\n";
    while(connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1){
    }
    isServerReady=true;
}
void display()
{
    //rendering
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

    glOrtho(0,1200,0,700,-1,1);

    glBegin(GL_QUADS);

    glColor3f(1.0,0.0,0.0);
    glVertex2f(mypos.x-width,mypos.y-width);
    glVertex2f(mypos.x-width,mypos.y+width);
    glVertex2f(mypos.x+width,mypos.y+width);
    glVertex2f(mypos.x+width,mypos.y-width);

    glColor3f(0.0,0.0,1.0);
    glVertex2f(otherpos.x-width,otherpos.y-width);
    glVertex2f(otherpos.x-width,otherpos.y+width);
    glVertex2f(otherpos.x+width,otherpos.y+width);
    glVertex2f(otherpos.x+width,otherpos.y-width);

    glEnd();

    glPopMatrix();

    SDL_GL_SwapBuffers();
}


int main(int argc,char *args[])
{
    init();

    bool isrunning=true;
    SDL_Event event;
    int old_time,new_time,delta;

    bool myturn=false;
    while(isrunning)
    {
        old_time = SDL_GetTicks();
        while(SDL_PollEvent(&event));
        {
            if(event.type==SDL_QUIT)
                isrunning=false;
            else if(event.type==SDL_KEYDOWN&&event.key.keysym.sym==SDLK_ESCAPE)
            isrunning=false;
        }

        //logic
        handleKeys();
        if(mypos.x-width<0.0f)
            mypos.x+=5*width;
        else if(mypos.x+width >1200.0f)
            mypos.x-=5*width;

        if(mypos.y-width<0.0f)
            mypos.y+=5*width;
        else if(mypos.y+width >700.0f)
            mypos.y-=5*width;

        //network code
        int n;
        if(isServerReady){
            if(myturn){
                read(clientSocket,(void *)&otherpos,sizeof(position));
                myturn=false;
            }
            else{
                write(clientSocket,(void*)&mypos,sizeof(position));
                myturn=true;
            }

        }

        if(cc(mypos.x,mypos.y,width,width,otherpos.x,otherpos.y,width,width))
        {
            SDL_Delay(2000);
            mypos.x = 20.0f;
            mypos.y = 20.0f;
            otherpos.x = 1180.0f;
            otherpos.y = 680.0f;
        }
        display();
        new_time = SDL_GetTicks();
        delta=new_time-old_time;
        if(delta < 4){
            SDL_Delay(4-delta);
        }
    }
    return 0;
}

