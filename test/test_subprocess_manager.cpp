#include <stdexcept>
#include <subprocess_manager.h>
#include <format>
#include <iostream>
#include "utest.h"
using namespace subprocess_manager;
std::string get_exe_path(){
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    return std::string(buffer);
}

std::string get_time(){
    time_t _time = time(NULL);
    struct tm _tm = *localtime(&_time);
    char message[100];
    sprintf(message,"%d-%02d-%02d %02d:%02d:%02d",
        _tm.tm_year + 1900,
        _tm.tm_mon  + 1,
        _tm.tm_mday,
        _tm.tm_hour,
        _tm.tm_min,
        _tm.tm_sec
    );
    return std::string(message);
}
UTEST(Task, Task)
{
    for(int i=0;i<10;i++){  // run 10 times
        std::cout << get_time() << std::endl;
        Sleep(100); // sleep for 100ms
    }
    EXPECT_EQ(0, 0);
}
std::string task_cmd = std::format("{0} --filter=Task.Task",get_exe_path());
UTEST(Subprocess, SingleProcess)
{
    Subprocess *process1 = new Subprocess(task_cmd);
    process1->start();
    EXPECT_EQ(process1->m_return_code, 0);
    delete process1;
}

UTEST(Subprocess, TwoProcess)
{
    Subprocess *process1 = new Subprocess(task_cmd);
    Subprocess *process2 = new Subprocess(task_cmd);
    process1->start_async();
    process2->start_async();
    while(true){
        if(!process1->m_active && !process2->m_active){
            break;
        }
        Sleep(100);
    }
    
    EXPECT_EQ(process1->m_return_code, 0);
    EXPECT_EQ(process2->m_return_code, 0);
    delete process1;
}

UTEST(Subprocess, InvalidTaskPath)
{
    Subprocess *process1 = new Subprocess("invalidname.exe 1 100 1");
    EXPECT_EXCEPTION({process1->start();},std::runtime_error);
}

UTEST(SubprocessManager, SingleProcess)
{
    SubprocessManager manager;
    manager.add("task1", task_cmd);
    manager.start();
    EXPECT_EQ(manager["task1"]->m_return_code, 0);
}
UTEST(SubprocessManager, TwoProcess)
{
    SubprocessManager manager;
    manager.add("task1", task_cmd);
    manager.add("task2", "task.exe 1 100 2");
    manager.start();
    EXPECT_EQ(manager["task1"]->m_return_code, 0);
    EXPECT_EQ(manager["task2"]->m_return_code, 0);
}
UTEST(SubprocessManager, InvalidTask)
{
    SubprocessManager manager;
    manager.add("task1", "invalidname.exe 1 100 1");
    EXPECT_EXCEPTION({manager.start();},std::runtime_error);
}

UTEST(SubprocessManager, DuplicateTask)
{
    SubprocessManager manager;
    manager.add("task1", task_cmd);
    EXPECT_EXCEPTION({manager.add("task1", task_cmd);},std::runtime_error);
}

UTEST_MAIN();