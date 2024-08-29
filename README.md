# subprocess_manager
The subprocess_manager library provides a simple and convenient way to manage subprocesses in C++. It allows you to easily create, start, and monitor subprocesses asynchronously, as well as retrieve their output and return codes.

### Example Subprocess
```cpp
#include <subprocess_manager.h>
using namespace subprocess_manager;
int main() {
  // Create a subprocess
  Subprocess subprocess("my_subprocess","my_command", "/path/to/working/directory","/path/to/log/file",{{"env_var","value"}});

  // Start the subprocess
  // subprocess.start(); // block until all subprocesses are done
  subprocess.start_async();

  // Wait for the subprocess to complete or do something else
  while(subprocess.m_state == Subprocess_Completed);

  // Get the return code and output from the subprocess
  std::cout << "Return code : " << subprocess.m_return_code << std::endl;
  std::cout << "Output      : " << subprocess.m_output_str << std::endl;
  std::cout << "Duration    : " << subprocess.m_duration << std::endl;
  std::cout << "Process Id  : " << subprocess.m_process_id << std::endl;
  
  return 0;
}
```
### Example SubprocessManager
```cpp
#include <subprocess_manager.h>
using namespace subprocess_manager;
int main() {
  // Create a subprocess manager
  SubprocessManager manager;

  // Add a subprocess to the manager
  // add(name, cmd, working_directory="", log_file_path="")
  manager.add("my_subprocess1", "my_command1", "/path/to/working/directory","/path/to/log/file",{{"env_var","value"}});
  manager.add("my_subprocess2", "my_command1", "/path/to/working/directory","/path/to/log/file");
  manager.add("my_subprocess3", "my_command2", "/path/to/working/directory"); 
  manager.add("my_subprocess4", "my_command3"); 

  // Start the subprocesses
  // manager.start(); // block until all subprocesses are done
  manager.start_async();

  // Wait for the subprocesses to complete or do something else
  while(manager.m_state == Subprocess_Completed);

  // Get the return code and output from the subprocesses
  std::cout << "my_subprocess1 (return code) : " << manager["my_subprocess1"]->m_return_code << std::endl;
  std::cout << "my_subprocess2 (return code) : " << manager["my_subprocess2"]->m_return_code << std::endl;
  std::cout << "my_subprocess3 (return code) : " << manager["my_subprocess3"]->m_return_code << std::endl;
  
  return 0;
}
```


## API
The subprocess_manager library provides the following classes and functions:

### Subprocess
- **Subprocess**: Represents a single subprocess.
- **start()**: Starts the subprocess.
- **start_async()**: Starts the subprocess asynchronously.
- **terminate**: Terminates the subprocess.
- **join**: Waits for the subprocess to complete.

### SubprocessManager
- **SubprocessManager**: Manages a collection of subprocesses.
- **add(name, command, curr_directory)**: Adds a new subprocess to the manager.
- **add(name, Subprocess\*)**: Adds an existing subprocess to the manager.
- **find(name)**: Finds a subprocess by name.
- **start()**: Starts all of the subprocesses in the manager.
- **start_async()**: Starts all of the subprocesses in the manager asynchronously.
- **operator[]**: Returns a reference to the subprocess with the given name.
- **terminate**: Terminates all of the subprocesses in the manager.
- **join**: Waits for all of the subprocesses in the manager to complete.

#### Enum:Subprocess_
 - **Subprocess_NotStart** : The subprocess has not been started.
 - **Subprocess_Started** : The subprocess has been started.
 - **Subprocess_InProgress** : The subprocess is currently running.
 - **Subprocess_Completed** : The subprocess has completed.
 - **Subprocess_Terminated** : The subprocess has been terminated.

## External Libraries
The subprocess_manager library relies on the following external libraries:

- **[utest](https://github.com/sheredom/utest.h):** A lightweight unit testing framework used to ensure the reliability and correctness of this project.
