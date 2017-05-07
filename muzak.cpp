/**
 * (c) 2012-2017 Marc Santoro <marc.santoro@gmail.com>
 * Released under the GPLv2 or GPLv3
 */

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <pthread.h>
#include <iostream>
#include <signal.h>
#include <dirent.h>
#include <vector>
#include <unistd.h>


using namespace std;

vector<string> files;
pthread_t muzak_thr;
pthread_mutex_t muzak_mtx;
pthread_cond_t muzak_cond;

#define INDEX_ROOT "/home/marc/Music"

void buildMusicIndexSubdir(const char *prefix, DIR *root) {
  DIR *node;
  dirent *dir;

  while(dir = readdir(root)) {
    if(!strncmp(dir->d_name, ".", 1) || !strncmp(dir->d_name, "..", 2))
      continue;
    char subpath[PATH_MAX];
    snprintf(subpath, PATH_MAX, "%s/%s", prefix, dir->d_name);
    if(dir->d_type == DT_DIR) {
      cout << "Recursing into directory " << subpath << endl;
      node = opendir(subpath);
      if(node) {
	buildMusicIndexSubdir(subpath, node);
	closedir(node);
      }
    } else if(dir->d_type == DT_REG) {
      cout << "Found file: " << subpath << endl;
      // TODO - look at extension of file?
      files.push_back(subpath);
    }
  }
}
  
void buildMusicIndex(const char *rootpath) {
  DIR *root;
  root = opendir(rootpath);
  buildMusicIndexSubdir(rootpath, root);
  cout << "Built index..." << files.size() << " entries." << endl;
}

void toggleMuzak() {
  cout << "Toggling muzak mode..." << endl;
  pthread_mutex_lock(&muzak_mtx);
  pthread_cond_signal(&muzak_cond);
  pthread_mutex_unlock(&muzak_mtx);
}

void run_child_process(int idx) {
  const char *rand_file = files[idx].c_str();
  execlp("mplayer", "mplayer", "-quiet", rand_file, NULL);
  cout << "In child process..." << endl;
  sleep(3);
  cout << "Exiting child process..." << endl;
  exit(0);
}

void *muzak_thread_func(void *arg) {
  pid_t child_pid;
  bool playing = false;

  cout << "Muzak thread initialized." << endl;
  while(true) {
    pthread_cond_wait(&muzak_cond, &muzak_mtx);
    cout << "Thread cycling..." << playing << endl;
    if(!playing) {
      // we have to generate the index here
      // otherwise, since run_child_process gets called in a 
      // child process, rand() below fork will always
      // return the same value
      int idx = rand() % files.size();
      pid_t child = fork();
      if(child) {
	child_pid = child;
	playing = true;
      } else {
	run_child_process(idx);
      }
    } else {
      cout << "Killing child process " << child_pid << endl;
      kill(child_pid, SIGTERM);
      usleep(100000);
      kill(child_pid, SIGKILL);
      playing = false;
    }
  }
}

void init_muzak_thread() {
  pthread_mutex_init(&muzak_mtx, NULL);
  pthread_cond_init(&muzak_cond, NULL);
  pthread_create(&muzak_thr, NULL, muzak_thread_func, NULL);
}


void init_joysticks() {
  int jsCount = SDL_NumJoysticks();
  printf("Detected %i joysticks\n", jsCount);
  for(int i=0;i<jsCount;i++) {
    SDL_Joystick *s = SDL_JoystickOpen(i);
    if(s)
      printf("Opened joystick %s (%i)\n", SDL_JoystickNameForIndex(i), i);
    else
      printf("Failed to open joystick %s (%i)\n", SDL_JoystickNameForIndex(i), i);
  }
  
}

void muzak_loop() {
  int running = true;
  int start = time(NULL);
  while(start == time(NULL)) {
    SDL_Event e;
    SDL_PollEvent(&e);
  }

  while(running) {
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
  srand(time(NULL));
  ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);

  printf("SDL Initialized: %d\n", ret);
  if(argc == 1)
    buildMusicIndex(INDEX_ROOT);
  else
    buildMusicIndex(argv[1]);
  if(files.size() == 0) {
    cout << "Error: unable to load any audio files." << endl;
    return 0;
  }
  init_joysticks();
  init_muzak_thread();
  muzak_loop();
  
  return 0;
}

