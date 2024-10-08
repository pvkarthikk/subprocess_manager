#ifndef SUBPROCESS_MANAGER_H    // Include guard to prevent multiple definitions
#define SUBPROCESS_MANAGER_H
#include <windows.h>            // For Windows API functions (process management)
#include <string>               // For string manipulation
#include <vector>               // For dynamic arrays
#include <thread>               // For multithreading
#include <unordered_map>        // For efficient key-value storage
#include <ctime>                // For time-related operations
#include <map>
namespace subprocess_manager {  // Namespace to encapsulate subprocess management functionality
    enum Subprocess_{
        Subprocess_NotStarted,
        Subprocess_Started,
        Subprocess_InProgress,
        Subprocess_Completed,
        Subprocess_Terminated
    };
    class Subprocess {
        private:
            // parameters
            STARTUPINFO                                 m_si;               // Startup information for the process
            PROCESS_INFORMATION                         m_pi;               // Process information (ID, handles)
            HANDLE                                      m_hRead;            // Read handle for the process's output
            HANDLE                                      m_hWrite;           // Write handle for the process's input
            std::thread*                                p_monitor_thread;   // Pointer to the monitoring thread
            clock_t                                     m_start_time;       // Start time of the process
            std::map<std::string,std::string>           m_env_var;          // Environment variables for the process
            // apis
            void                                        monitor();          // Function to monitor process output
            void                                        execute();          // Function to execute process
        public:
            // parameters
            std::string                                 m_name;             // Name of the process
            std::string                                 m_command;          // Command to be executed
            std::string                                 m_curr_directory;   // Current working directory of the process
            std::string                                 m_log_path;         // Log file path
            std::string                                 m_output_str;       // String representation of process output
            std::vector<std::string>                    m_output;           // Vector to store process output lines
            double                                      m_duration;         // Duration of the process
            int                                         m_process_id;       // Process ID
            int                                         m_return_code;      // Return code of the process
            Subprocess_                                 m_state;            // State of the process
            // apis
            Subprocess*                                 start();            // Function to start the process
            Subprocess*                                 start_async();      // Function to start the process asynchronosly
            Subprocess*                                 terminate();        // Function to terminate the process
            Subprocess*                                 join();             // Function to join the monitoring thread
            Subprocess( std::string name,
                        std::string command,
                        std::string curr_directory="",
                        std::string log_path="",
                        std::map<std::string,std::string> env_var={{}});    // Constructor
            ~Subprocess();                                                  // Destructor
    };

    class SubprocessManager {
        private:
            std::thread*                                p_monitor_thread;   // Pointer to the monitoring thread
            void                                        monitor();          // Function to monitor subprocesses
            void                                        execute();          // Function to execute subprocesses
        public:
            std::vector<Subprocess*>                    m_processes;        // Vector to store subprocesses
            Subprocess_                                 m_state;    // State of the manager
            int                                         find(std::string name); // Find a subprocess by name
            SubprocessManager*                          start();            // Function to start the manager and its subprocesses
            SubprocessManager*                          start_async();      // Function to start the manager and its subprocesses asynchronously
            SubprocessManager*                          terminate();        // Function to terminate all subprocesses
            SubprocessManager*                          join();             // Function to join the monitoring thread
            Subprocess*                                 operator[](std::string name); // Access a subprocess by name
            SubprocessManager*                          add(Subprocess* process); // Add a subprocess
            SubprocessManager*                          add(std::string name,
                                                            std::string command,
                                                            std::string curr_directory="",
                                                            std::string log_path="",
                                                            std::map<std::string,std::string> env_var={{}}); // Add a subprocess
            SubprocessManager();                                            // Constructor
            ~SubprocessManager();                                           // Destructor
    };
}

#endif // SUBPROCESS_MANAGER_H
