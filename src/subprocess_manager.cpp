#include <subprocess_manager.h>
#include <format>
#include <functional>
#include <iostream>
#include <fstream>
using namespace subprocess_manager;
Subprocess::Subprocess(std::string command, std::string curr_directory, std::string log_path)
{
    this->m_command = command;
    this->m_curr_directory = curr_directory;
    this->m_log_path = log_path;
    this->m_started = false;
    this->m_active = false;
    this->m_process_id = -1;
    this->m_return_code = -1;
    this->p_monitor_thread = nullptr;
    this->m_output = {};
    this->m_monitor_flag = false;
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
    }
}
void Subprocess::start(){
    this->execute();
    this->monitor();
}
void Subprocess::start_async(){
    this->execute();
    // initiate monitor thread to monitor the process
    this->p_monitor_thread = new std::thread(std::bind(&Subprocess::monitor, this));
}
void Subprocess::execute(){
    if(this->m_started){
        throw std::runtime_error(std::format("'{0}' already running",this->m_command));
    }
    this->m_start_time = clock();
    this->m_started = true;
    this->m_active = true;
    this->m_output = {};
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
        0,
        NULL,
        lpCurrDir,
        &this->m_si,
        &this->m_pi
        )
    ) {
        // Handle error
        throw std::runtime_error("Unable to create process");
    }
    // Close handle to the write end of the pipe.
    // No longer needed by the parent process.
    CloseHandle(this->m_hWrite);
    // update process id
    this->m_process_id = this->m_pi.dwProcessId;
    // start monitoring
    this->m_monitor_flag = true;
}

void Subprocess::monitor()
{
    std::ofstream log_file;
    // if log is specified, open the log file
    if(this->m_log_path != ""){
        log_file.open(this->m_log_path); 
        log_file.close();
    }
    while (this->m_monitor_flag) {
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
            // add current buffer to output stream
            this->m_output.push_back(out_str);
            // if log is specified, write to log file
            if(this->m_log_path != ""){
                out_str.pop_back();
                log_file.open(this->m_log_path,std::ios::app);
                log_file << std::string(buffer);
                log_file.close();
            }
        }
        Sleep(100); // Adjust the sleep interval as needed
    }
    auto end = clock();
    this->m_duration = double(this->m_start_time - end)/CLOCKS_PER_SEC ;
    this->m_monitor_flag = false;
    this->m_active = false;
}
void Subprocess::terminate(){
    this->m_monitor_flag = false;
    if(this->p_monitor_thread != nullptr){
        if(this->p_monitor_thread->joinable()){
            this->p_monitor_thread->join();
        }
        delete this->p_monitor_thread;
    }
}
SubprocessManager::SubprocessManager(){
    this->m_active =false;
    this->m_started = false;
    this->m_monitor_flag = false;
    this->p_monitor_thread = NULL;
    this->m_process_names = {};
    this->m_processes = {};
}
SubprocessManager::~SubprocessManager(){
    for(const auto& _pair:this->m_processes){
        delete _pair.second;
    }
    if(this->p_monitor_thread != nullptr){
        if(this->p_monitor_thread->joinable()){
            this->p_monitor_thread->join();
        }
        delete this->p_monitor_thread;
    }
}
void SubprocessManager::add(std::string name, Subprocess* process)
{
    if(this->m_processes.find(name) != this->m_processes.end()){
        throw std::runtime_error(std::format("Duplicate task found('{0}')",name));
    }
    this->m_processes[name] = process;
    this->m_process_names.push_back(name);
}
void SubprocessManager::add(std::string name, std::string command, std::string curr_directory, std::string log_path)
{
    if(this->m_processes.find(name) != this->m_processes.end()){
        throw std::runtime_error(std::format("Duplicate task found('{0}')",name));
    }
    this->m_processes[name] = new Subprocess(command,curr_directory,log_path);
    this->m_process_names.push_back(name);
}
Subprocess* SubprocessManager::operator[](std::string name){
    if(this->m_processes.find(name) == this->m_processes.end()){
        throw std::runtime_error(std::format("Task '{0}' not found in the manager",name));
    }
    return this->m_processes[name];
}
void SubprocessManager::start(){
    this->execute();
    this->monitor();
}
void SubprocessManager::start_async(){
    this->execute();
    this->p_monitor_thread = new std::thread(std::bind(&SubprocessManager::monitor, this));
}
void SubprocessManager::execute(){
    if(this->m_started){
        throw std::runtime_error("Manager already started");
    }
    this->m_started = true;
    this->m_active  = true;
    for(const auto& _pair:this->m_processes){
        _pair.second->start_async();
    }
    this->m_monitor_flag = true;
}
void SubprocessManager::monitor(){
    while(this->m_monitor_flag){
        bool _all_done = true;    
        for(const auto& _pair:this->m_processes){
            if(_pair.second->m_active){
                _all_done = false;
            }
        }
        if(_all_done){
            break;
        }
        Sleep(100); 
    }
    this->m_active = false;
    this->m_monitor_flag = false;
}
void SubprocessManager::terminate(){
    this->m_monitor_flag = false;
    for(const auto& _pair:this->m_processes){
        _pair.second->terminate();
    }
    if(this->p_monitor_thread != nullptr){
        if(this->p_monitor_thread->joinable()){
            this->p_monitor_thread->join();
        }
        delete this->p_monitor_thread;
    }
}