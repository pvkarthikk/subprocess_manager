#include <subprocess_manager.h>
#include <format>
#include <functional>
#include <iostream>
#include <fstream>
using namespace subprocess_manager;
Subprocess::Subprocess(std::string name, std::string command, std::string curr_directory, std::string log_path)
{
    this->m_command = command;
    this->m_curr_directory = curr_directory;
    this->m_log_path = log_path;
    this->m_process_id = -1;
    this->m_return_code = -1;
    this->p_monitor_thread = nullptr;
    this->m_output = {};
    this->m_output_str = "";
    this->m_name = name;
    this->m_state = Subprocess_NotStarted;
    this->m_duration = 0.0;
    ZeroMemory(&this->m_pi, sizeof(this->m_pi));
    ZeroMemory(&this->m_si, sizeof(this->m_si));
}
Subprocess::~Subprocess(){
    CloseHandle(this->m_pi.hProcess);
    CloseHandle(this->m_pi.hThread);
    CloseHandle(this->m_hRead);
    CloseHandle(this->m_hWrite);
    if(this->p_monitor_thread != nullptr){
        if(this->p_monitor_thread->joinable()){
            this->p_monitor_thread->join();
        }
        delete this->p_monitor_thread;
        this->p_monitor_thread = nullptr;
    }
}
void Subprocess::start(){
    this->execute();
    this->monitor();
}
void Subprocess::start_async(){
    this->execute();
    // initiate monitor thread to monitor the process
    this->p_monitor_thread = new std::jthread(std::bind(&Subprocess::monitor, this));
}
void Subprocess::execute(){
    if(this->m_state == Subprocess_Started){
        throw std::runtime_error(std::format("'{0}' already running",this->m_command));
    }
    this->m_state = Subprocess_Started;
    this->m_start_time = clock();
    this->m_output = {};
    this->m_output_str = "";
    this->m_return_code = -1;
    // update security attribs
    SECURITY_ATTRIBUTES saAttr;
    BOOL fSuccess;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT.
    fSuccess = CreatePipe(&this->m_hRead, &this->m_hWrite, &saAttr, 0);
    if (!fSuccess) {
        // Handle error
        throw std::runtime_error("Unable to create r/w pipe");
    }
    // Ensure the read handle to the pipe for STDOUT is not inherited.
    fSuccess = SetHandleInformation(this->m_hRead, HANDLE_FLAG_INHERIT, 0);
    if (!fSuccess) {
        // Handle error
        throw std::runtime_error("Unable to create pipe to communicate with child process");
    }

    // Create the child process.
    this->m_si.cb = sizeof(this->m_si);
    this->m_si.dwFlags |= STARTF_USESTDHANDLES;
    this->m_si.wShowWindow = SW_HIDE;
    this->m_si.hStdOutput = this->m_hWrite;
    // Replace with your desired command
    LPSTR lpCmdline = const_cast<char *>(this->m_command.c_str());
    LPSTR lpCurrDir = NULL;
    if(this->m_curr_directory != ""){
        lpCurrDir = const_cast<char*>(this->m_curr_directory.c_str());
    }
    if (!CreateProcess(
        NULL,
        lpCmdline,
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        lpCurrDir,
        &this->m_si,
        &this->m_pi
        )
    ) {
        // Handle error
        throw std::runtime_error(std::format("Unable to create process '{0}'",lpCmdline));
    }
    // Close handle to the write end of the pipe.
    // No longer needed by the parent process.
    CloseHandle(this->m_hWrite);
    // update process id
    this->m_process_id = this->m_pi.dwProcessId;
    // start monitoring
    this->m_state = Subprocess_InProgress;
}

void Subprocess::monitor()
{
    std::ofstream log_file;
    // if log is specified, open the log file
    if(this->m_log_path != ""){
        log_file.open(this->m_log_path); 
    }
    while (this->m_state == Subprocess_InProgress) {
        DWORD exitCode;
        if (!GetExitCodeProcess(this->m_pi.hProcess, &exitCode)) {
            this->m_return_code = -2;
            break;
        }

        if (exitCode != STILL_ACTIVE) {
            this->m_return_code = (int)exitCode;
            break;
        }
        // Read from pipe until end-of-file is reached.
        char buffer[4096];
        DWORD dwRead;
        while (ReadFile(this->m_hRead, buffer, sizeof(buffer) - 1, &dwRead, NULL) && dwRead != 0) {
            buffer[dwRead] = '\0';
            std::string out_str = std::string(buffer);
            this->m_output_str += out_str;
            // remove trailing newline
            if(out_str.back() == '\n'){
                out_str.pop_back();
            }
            // add current buffer to output stream
            this->m_output.push_back(out_str);
            // if log is specified, write to log file
            if(this->m_log_path != ""){
                log_file << std::string(buffer);
                log_file.flush();
            }
        }
    }
    if(this->m_log_path != ""){
        log_file.close();
    }
    auto end = clock();
    this->m_duration = double(this->m_start_time - end)/CLOCKS_PER_SEC ;
    this->m_state = Subprocess_Completed;
}
void Subprocess::join(){
    if(this->p_monitor_thread != nullptr){
        if(this->p_monitor_thread->joinable()){
            this->p_monitor_thread->join();
        }
    }
}
void Subprocess::terminate(){
    this->join();
    if(this->p_monitor_thread != nullptr){
        delete this->p_monitor_thread;
        this->p_monitor_thread = nullptr;
    }
    this->m_state = Subprocess_Terminated;
}
SubprocessManager::SubprocessManager(){
    this->m_state = Subprocess_NotStarted;
    this->p_monitor_thread = NULL;
    this->m_processes = {};
}
SubprocessManager::~SubprocessManager(){
    for(Subprocess *process:this->m_processes){
        delete process;
    }
    if(this->p_monitor_thread != nullptr){
        if(this->p_monitor_thread->joinable()){
            this->p_monitor_thread->join();
        }
        delete this->p_monitor_thread;
        this->p_monitor_thread = nullptr;
    }
}
int SubprocessManager::find(std::string name){
    for(int i=0;i<this->m_processes.size();i++){
        if(this->m_processes[i]->m_name == name){
            return i;
        }
    }
    return -1;
}
void SubprocessManager::add(Subprocess* process)
{
    if(process == nullptr){
        throw std::runtime_error("Given process is 'NULL'");
    }
    if(this->find(process->m_name) != -1){
        throw std::runtime_error(std::format("Duplicate task found('{0}')",process->m_name));
    }
    this->m_processes.push_back(process);
}
void SubprocessManager::add(std::string name, std::string command, std::string curr_directory, std::string log_path)
{
    if(this->find(name) != -1){
        throw std::runtime_error(std::format("Duplicate task found('{0}')",name));
    }
    
    this->m_processes.push_back(new Subprocess(name,command,curr_directory,log_path));
}
Subprocess* SubprocessManager::operator[](std::string name){
    int found_idx = this->find(name);
    if( found_idx == -1){
        throw std::runtime_error(std::format("Task '{0}' not found in the manager",name));
    }
    return this->m_processes[found_idx];
}
void SubprocessManager::start(){
    this->execute();
    this->monitor();
}
void SubprocessManager::start_async(){
    this->execute();
    this->p_monitor_thread = new std::jthread(std::bind(&SubprocessManager::monitor, this));
}
void SubprocessManager::execute(){
    if(this->m_state == Subprocess_Started){
        throw std::runtime_error("Manager already started");
    }
    this->m_state = Subprocess_Started;
    for(Subprocess *process:this->m_processes){
        process->start_async();
    }
    this->m_state = Subprocess_InProgress;
}
void SubprocessManager::monitor(){
    while(this->m_state == Subprocess_InProgress){
        bool _all_done = true;    
        for(Subprocess *process:this->m_processes){
            if(process->m_state != Subprocess_Completed){
                _all_done = false;
            }
        }
        if(_all_done){
            break;
        }
        Sleep(100); 
    }
    this->m_state = Subprocess_Completed;
}
void SubprocessManager::join(){
    for(Subprocess *process:this->m_processes){
        process->join();
    }
    if(this->p_monitor_thread != nullptr){
        if(this->p_monitor_thread->joinable()){
            this->p_monitor_thread->join();
        }
    }
}
void SubprocessManager::terminate(){
    this->join();
    if(this->p_monitor_thread != nullptr){
        delete this->p_monitor_thread;
        this->p_monitor_thread = nullptr;
    }
    this->m_state = Subprocess_Terminated;
}