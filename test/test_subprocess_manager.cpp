#include <subprocess_manager.h>
#include <exception>
#include "utest.h"
UTEST(Subprocess, SingleProcess)
{
    Subprocess *process1 = new Subprocess("task.exe 1 100 1");
    process1->start();
    while(process1->m_active);
    EXPECT_EQ(process1->m_return_code, 1);
    delete process1;
}

UTEST(Subprocess, TwoProcess)
{
    Subprocess *process1 = new Subprocess("task.exe 1 100 1");
    Subprocess *process2 = new Subprocess("task.exe 1 100 2");
    process1->start();
    process2->start();
    while(process1->m_active || process2->m_active);
    EXPECT_EQ(process1->m_return_code, 1);
    EXPECT_EQ(process2->m_return_code, 2);
    delete process1;
}

UTEST(Subprocess, InvalidTaskPath)
{
    Subprocess *process1 = new Subprocess("invalidname.exe 1 100 1");
    EXPECT_EXCEPTION({process1->start()},std::exception);
}

UTEST(SubprocessManager, DuplicateTask)
{
    SubprocessManager manager;
    manager.add("task1", "task.exe 1 100 1");
    EXPECT_EXCEPTION({manager.add("task1", "task.exe 1 100 1")},std::exception);
}
UTEST(SubprocessManager, SingleProcess)
{
    SubprocessManager manager;
    manager.add("task1", "task.exe 1 100 1");
    manager.start();
    while(manager.m_active);
    EXPECT_EQ(manager["task1"]->m_return_code, 1);
}
UTEST(SubprocessManager, TwoProcess)
{
    SubprocessManager manager;
    manager.add("task1", "task.exe 1 100 1");
    manager.add("task2", "task.exe 1 100 2");
    manager.start();
    while(manager.m_active);
    EXPECT_EQ(manager["task1"]->m_return_code, 1);
    EXPECT_EQ(manager["task2"]->m_return_code, 2);
}
UTEST(SubprocessManager, InvalidTask)
{
    SubprocessManager manager;
    manager.add("task1", "invalidname.exe 1 100 1");
    EXPECT_EXCEPTION({manager.start()},std::exception);
}
UTEST(SubprocessManager, DuplicateTask)
{
    SubprocessManager manager;
    manager.add("task1", "task.exe 1 100 1");
    manager.add("task2", "task.exe 1 100 2");
    EXPECT_EXCEPTION({manager.add("task1", "task.exe 1 100 1")},std::exception);
}
UTEST_MAIN();