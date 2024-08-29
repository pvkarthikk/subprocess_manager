#include <stdexcept>
#include <subprocess_manager.h>
#include <format>
#include <filesystem>
#include <iostream>
#include "utest.h"
using namespace std;
using namespace subprocess_manager;

UTEST(Subprocess, SingleProcess)
{
    // happy test case
    Subprocess *process1 = new Subprocess("process1","task.exe 2 100 1");
    process1->start_async()->join();
    // while(process1->m_state != Subprocess_Completed);
    EXPECT_EQ(1, process1->m_return_code);
    delete process1;
    // different directory
    Subprocess *diff_dir = new Subprocess("diff_dir","task.exe 2 100 1","..\\test");
    diff_dir->start();
    EXPECT_EQ(1, diff_dir->m_return_code);
    // log path
    Subprocess *log_path = new Subprocess("log_path","task.exe 2 100 1","","log.txt");
    log_path->start();
    EXPECT_EQ(1, log_path->m_return_code);
    EXPECT_TRUE(std::filesystem::exists("log.txt"));
    // env path
    Subprocess *env_path = new Subprocess("env_path","..\\test\\test_env_1.bat","..\\test\\","",{{"ENV1_VAR","ENV1_Value"}});
    EXPECT_EQ(env_path->start()->m_return_code, 0);
    EXPECT_STREQ(env_path->m_output_str.c_str(),"ENV1_Value\r\n");
    // invalid process
    Subprocess *invalid = new Subprocess("invalid","invalid 1 2 3");
    EXPECT_EXCEPTION({invalid->start_async();}, std::runtime_error);
    
    
}

UTEST(Subprocess, Multiprocess)
{
    Subprocess *process1 = new Subprocess("process1","task.exe 2 100 1");
    Subprocess *process2 = new Subprocess("process2","task.exe 4 100 2");
    process1->start_async();
    process2->start_async();
    while(true){
        if(process1->m_state == Subprocess_Completed && process2->m_state == Subprocess_Completed){
            break;
        }
        // Sleep(100);
    }
    EXPECT_EQ(1, process1->m_return_code);
    EXPECT_EQ(2, process2->m_return_code);
    delete process1;
    delete process2;
}
UTEST(SubprocessManager, Multiprocess)
{
    SubprocessManager manager = SubprocessManager();
    Subprocess *process1 = new Subprocess("process1","task.exe 2 100 1");
    manager.add(process1);
    manager.add("process2","task.exe 3 100 2");
    EXPECT_EXCEPTION({manager.add(process1);},std::runtime_error);
    EXPECT_EXCEPTION({manager.add("process2","task.exe 3 100 2");},std::runtime_error);
    manager.start_async();
    EXPECT_EXCEPTION({manager.start();},std::runtime_error);
    manager.join();
    EXPECT_EQ(1, manager["process1"]->m_return_code);
    EXPECT_EQ(2, manager["process2"]->m_return_code);
    EXPECT_EXCEPTION({manager["invalidprocess"];},std::runtime_error);
    manager.terminate();
    SubprocessManager *manager1 =new SubprocessManager();
    EXPECT_EQ(
        {
            SubprocessManager().add("process1","task.exe 2 100 1")
                ->add("process2","task.exe 3 100 2")
                ->start()
                ->join()
                ->terminate()
                ->m_state;
        }, Subprocess_Terminated
    );
}
UTEST_MAIN();