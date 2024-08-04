#include <iostream>
#include <time.h>
#include <windows.h>

using namespace std;

/** @brief get current time */
string get_time(){
    time_t _time = time(NULL);
    struct tm _tm = *localtime(&t);
    char message[100];
    sprintf(message,"%d-%02d-%02d %02d:%02d:%02d\n",
        _tm.tm_year + 1900,
        _tm.tm_mon  + 1,
        _tm.tm_mday,
        _tm.tm_hour,
        _tm.tm_min,
        _tm.tm_sec
    );
    return string(message);
}
int main(int argc, char** argv){
    // task.exe <count> <sleep> <exit_code>
    if(argc == 4){
        cerr << "Invalid no of commands" << endl;
        cerr << "task.exe <count> <sleep> <exit_code>" << endl;
    }
    int _count = atoi(argv[1]);
    int _sleep_time = atoi(argv[2]);
    int _exit_code = atoi(argv[3]);
    for(int i=0; i<_count; i++){
        cout << get_time() << endl;
        Sleep(_sleep_time);
    }
    return _exit_code;
}