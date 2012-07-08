#include <stdio.h>

#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <pthread.h>
#include <iostream>

using namespace std;

pthread_t muzak_thr;
pthread_mutex_t muzak_mtx;


void toggleMuzak() {
  cout << "Toggling muzak mode..." << endl;
}

void *muzak_thread_func(void *arg) {
  cout << "Muzak thread initialized." << endl;

}

void init_muzak_thread() {
  pthread_create(&muzak_thr, NULL, muzak_thread_func, NULL);
}


void init_joysticks() {
  int jsCount = SDL_NumJoysticks();
  printf("Detected %i joysticks\n", jsCount);
  for(int i=0;i<jsCount;i++) {
    SDL_Joystick *s = SDL_JoystickOpen(i);
    int ret = SDL_JoystickOpened(i);
    if(ret)
      printf("Opened joystick %s (%i:%i)\n", SDL_JoystickName(i), i, ret);
    else
      printf("Failed to open joystick %s (%i:%i)\n", SDL_JoystickName(i), i, ret);
  }
  
}

void muzak_loop() {
  while(true) {
    SDL_Event e;
    int ret = SDL_WaitEvent(&e);
    if(ret) {
      switch(e.type) {
      case SDL_JOYBUTTONDOWN:
	toggleMuzak();
	break;
      }
    } else {
      cerr << "Error while waiting for events; trying to continue..." << ret << endl;
    }
  }
    
}


int main(int argc, char **argv) {
  int ret = 0;

  ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);

  printf("SDL Initialized: %d\n", ret);
  init_joysticks();
  init_muzak_thread();
  muzak_loop();
  
  return 0;
}

