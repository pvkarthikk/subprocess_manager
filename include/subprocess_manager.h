#ifndef SUBPROCESS_MANAGER_H    // Include guard to prevent multiple definitions
#define SUBPROCESS_MANAGER_H
#include <windows.h>            // For Windows API functions (process management)
#include <string>               // For string manipulation
#include <vector>               // For dynamic arrays
#include <thread>               // For multithreading
#include <unordered_map>        // For efficient key-value storage
#include <chrono>               // For time-related operations

namespace subprocess_manager {  // Namespace to encapsulate subprocess management functionality

    class Subprocess {
        private:
            // parameters
            STARTUPINFO                                 m_si;               // Startup information for the process
            PROCESS_INFORMATION                         m_pi;               // Process information (ID, handles)
            HANDLE                                      m_hRead;            // Read handle for the process's output
            HANDLE                                      m_hWrite;           // Write handle for the process's input
            std::thread*                                p_monitor_thread;   // Pointer to the monitoring thread
            bool                                        m_started;          // Flag indicating if the process has started
            std::string                                 m_command;          // Command to be executed
            std::string                                 m_curr_directory;   // Current working directory of the process
            std::string                                 m_log_path;         // Log file path
            bool                                        m_monitor_flag;     // Flag to enable monitoring
            auto                                        m_start_time;       // Start time of the process
            // apis
            void                                        monitor();          // Function to monitor process output
            void                                        execute();          // Function to execute process
        public:
            // parameters
            int                                         m_process_id;       // Process ID
            int                                         m_return_code;      // Return code of the process
            bool                                        m_active;           // Flag indicating if the process is active
            std::vector<std::string>                    m_output;           // Vector to store process output lines
            auto                                        m_duration;         // Duration of the process
            // apis
            void                                        start();            // Function to start the process
            void                                        start_async();      // Function to start the process asynchronosly
            void                                        terminate();        // Function to terminate the process
            Subprocess(std::string command, std::string curr_directory="", std::string log_path="");    // Constructor
            ~Subprocess();                                                  // Destructor
    };

    typedef std::unordered_map<std::string, Subprocess*> SubprocessMap;     // Type alias for a map of subprocesses

    class SubprocessManager {
        private:
            SubprocessMap                               m_processes;        // Map to store subprocesses by name
            bool                                        m_started;          // Flag indicating if the manager has started
            std::thread*                                p_monitor_thread;   // Pointer to the monitoring thread
            bool                                        m_monitor_flag;     // Flag to enable monitoring
            void                                        monitor();          // Function to monitor subprocesses
            void                                        execute();          // Function to execute subprocesses
        public:
            std::vector<std::string>                    m_process_names;    // Vector of subprocess names
            bool                                        m_active;           // Flag indicating if the manager is active
            void                                        start();            // Function to start the manager and its subprocesses
            void                                        start_async();      // Function to start the manager and its subprocesses asynchronously
            void                                        terminate();        // Function to terminate all subprocesses
            Subprocess*                                 operator[](std::string name); // Access a subprocess by name
            void                                        add(std::string name, Subprocess* process); // Add a subprocess
            void                                        add(std::string name,
                                                            std::string command,
                                                            std::string curr_directory="",
                                                            std::string log_path=""); // Add a subprocess
            SubprocessManager();                                            // Constructor
            ~SubprocessManager();                                           // Destructor
    };
}

#endif // SUBPROCESS_MANAGER_H
