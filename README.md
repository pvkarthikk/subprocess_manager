# subprocess_manager
The subprocess_manager library provides a simple and convenient way to manage subprocesses in C++. It allows you to easily create, start, and monitor subprocesses asynchronously, as well as retrieve their output and return codes.

### Example Subprocess
```cpp
#include <subprocess_manager.h>

int main() {
  // Create a subprocess
  subprocess_manager::Subprocess subprocess("my_command", "/path/to/working/directory","/path/to/log/file");

  // Start the subprocess
  subprocess.start_async();

  // Wait for the subprocess to complete or do something else
  while(subprocess.m_active);

  // Get the return code and output from the subprocess
  std::cout << "Return code : " << subprocess.m_return_code << std::endl;
  std::cout << "Output      : " << std::endl;
  for(std::string line:subprocess.m_output){
    std::cout << line << std::endl;
  }
  return 0;
}
```
### Example SubprocessManager
```cpp
#include <subprocess_manager.h>

int main() {
  // Create a subprocess manager
  subprocess_manager::SubprocessManager manager;

  // Add a subprocess to the manager
  // add(name, cmd, working_directory="", log_file_path="")
  manager.add("my_subprocess1", "my_command1", "/path/to/working/directory","/path/to/log/file");
  manager.add("my_subprocess2", "my_command2", "/path/to/working/directory"); 
  manager.add("my_subprocess3", "my_command3"); 

  // Start the subprocesses
  // manager.start(); // block until all subprocesses are done
  manager.start_async();

  // Wait for the subprocesses to complete or do something else
  while(manager.m_active);

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

### SubprocessManager
- **SubprocessManager**: Manages a collection of subprocesses.
- **add(name, command, curr_directory)**: Adds a new subprocess to the manager.
- **add(name, Subprocess\*)**: Adds an existing subprocess to the manager.
- **start()**: Starts all of the subprocesses in the manager.
- **start_async()**: Starts all of the subprocesses in the manager asynchronously.
- **operator[]**: Returns a reference to the subprocess with the given name.
- **terminate**: Terminates all of the subprocesses in the manager.

