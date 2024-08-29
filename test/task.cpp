#include <iostream>
#include <chrono>
#include <format>
#include <windows.h>

using namespace std;

string get_time(){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char message[100];
  sprintf(message,"%d-%02d-%02d %02d:%02d:%02d", 
  tm.tm_year + 1900,
  tm.tm_mon + 1,
  tm.tm_mday,
  tm.tm_hour,
  tm.tm_min,
  tm.tm_sec
  );
  return std::string(message);
}

int main(int argc, char** argv)
{
  /**
   * argv: task.exe <count> <sleep> <exit_code>
   */
    if(argc != 4){
        cerr << "Invalid no of commands" << endl;
        cerr << "task.exe <count> <sleep> <exit_code>" << endl;
        return 1;
    }
    int count       = atoi(argv[1]);
    int sleep_time  = atoi(argv[2]);
    int exit_code   = atoi(argv[3]);
    for(int i=0;i<atoi(argv[1]);i++){
        cout << "Output:" << std::format("{0:%F_%T}",std::chrono::system_clock::now()) << endl;        
        Sleep(sleep_time);
    }
    if(exit_code != 0){
        cerr << "Error :" << std::format("{0:%F_%T}",std::chrono::system_clock::now());
        return exit_code;
    }
    return 0;
}