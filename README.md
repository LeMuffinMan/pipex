Todo 

parsing 
fork pour un pipe 
- pipe : to connect stdout of child to stdin of parent
- close unnecessary fd
- if a file is involved : access for permissions : Check if file1 exists and is readable, and if file2 can be created or written to.
- parent execute cmd2 with execve
- child execute cmd1 with execve
- dup2 : pour le pipe ET pour les stdout vers un file
- open : gerer erreurs
- read : gerer erreur
- close : gerer erreur 
- write : ecrire dans un fichier : gerer erreurs ?
- wait waitpid : pour pas laisser de orphenate
- pour TOUTES les erreurs : utiliser perror strerror 
- unlink : clean tmp files (option ?)


---

### **Function Explanations**

1. **`execve`**  
   - **Purpose**: Executes a program. It replaces the current process image with a new one specified by the path to the executable file.
   - **Usage**: `int execve(const char *pathname, char *const argv[], char *const envp[]);`  
   - **Example**: Used to run commands like `ls`, `grep`, or any other executable in the system.

2. **`access`**  
   - **Purpose**: Checks the accessibility of a file (e.g., whether it exists or has specific permissions).  
   - **Usage**: `int access(const char *pathname, int mode);`  
   - **Example**: Useful to verify if a file exists before trying to open or execute it.

3. **`perror`**  
   - **Purpose**: Prints an error message to `stderr` based on the current value of `errno`.  
   - **Usage**: `void perror(const char *s);`  
   - **Example**: Helps debug errors by providing descriptive messages (e.g., "Permission denied").

4. **`strerror`**  
   - **Purpose**: Returns a string describing the error code stored in `errno`.  
   - **Usage**: `char *strerror(int errnum);`  
   - **Example**: Useful for custom error handling and logging.

5. **`pipe`**  
   - **Purpose**: Creates a pipe, which is a unidirectional data channel for inter-process communication (IPC).  
   - **Usage**: `int pipe(int pipefd[2]);`  
   - **Example**: Used to connect the output of one process to the input of another (e.g., `cmd1 | cmd2`).

6. **`unlink`**  
   - **Purpose**: Deletes a file from the filesystem.  
   - **Usage**: `int unlink(const char *pathname);`  
   - **Example**: Can be used to clean up temporary files.

7. **`wait`**  
   - **Purpose**: Waits for any child process to terminate.  
   - **Usage**: `pid_t wait(int *wstatus);`  
   - **Example**: Ensures the parent process waits for its child processes to finish.

8. **`waitpid`**  
   - **Purpose**: Waits for a specific child process to terminate.  
   - **Usage**: `pid_t waitpid(pid_t pid, int *wstatus, int options);`  
   - **Example**: More control over waiting for specific child processes.

9. **`fork`**  
   - **Purpose**: Creates a new process by duplicating the calling process. The new process is called the child process.  
   - **Usage**: `pid_t fork(void);`  
   - **Example**: Used to create child processes to execute commands concurrently.

10. **`open` / `read` / `close`**  
    - **Purpose**:  
      - `open`: Opens a file and returns a file descriptor.  
      - `read`: Reads data from a file descriptor.  
      - `close`: Closes a file descriptor.  
    - **Usage**:  
      - `int open(const char *pathname, int flags);`  
      - `ssize_t read(int fd, void *buf, size_t count);`  
      - `int close(int fd);`  
    - **Example**: Used to read from or write to files.

11. **`dup2`**  
    - **Purpose**: Duplicates a file descriptor, allowing redirection of input/output.  
    - **Usage**: `int dup2(int oldfd, int newfd);`  
    - **Example**: Redirects `stdin` or `stdout` to a file or pipe.

---

### **How These Functions Help in the `pipex` Project**

The `pipex` project requires you to create a program that mimics the behavior of the shell pipe (`|`). For example:
```bash
$ ./pipex file1 cmd1 cmd2 file2
```
This should behave like:
```bash
$ < file1 cmd1 | cmd2 > file2
```

Here’s how the functions can help:

1. **`fork`**:  
   - Create child processes to execute `cmd1` and `cmd2` concurrently.

2. **`pipe`**:  
   - Create a pipe to connect the output of `cmd1` to the input of `cmd2`.

3. **`dup2`**:  
   - Redirect `stdin` and `stdout` of the commands to the pipe and files. For example:
     - Redirect `stdin` of `cmd1` to `file1`.
     - Redirect `stdout` of `cmd1` to the write end of the pipe.
     - Redirect `stdin` of `cmd2` to the read end of the pipe.
     - Redirect `stdout` of `cmd2` to `file2`.

4. **`execve`**:  
   - Execute `cmd1` and `cmd2` in the child processes.

5. **`open` / `read` / `close`**:  
   - Open `file1` and `file2`, read from `file1`, and write to `file2`.

6. **`wait` / `waitpid`**:  
   - Ensure the parent process waits for the child processes to finish executing.

7. **`access`**:  
   - Check if `file1` exists and is readable, and if `file2` can be created or written to.

8. **`perror` / `strerror`**:  
   - Handle errors gracefully by printing meaningful error messages.

9. **`unlink`**:  
   - Clean up temporary files if needed (though not strictly necessary for `pipex`).

---

### **Summary**

These functions are essential for:
- Creating and managing processes (`fork`, `execve`, `wait`, `waitpid`).
- Setting up communication between processes (`pipe`, `dup2`).
- Handling file I/O (`open`, `read`, `close`).
- Managing errors and debugging (`perror`, `strerror`, `access`).

By combining these functions, you can build a program that reads input from a file, passes it through multiple commands via pipes, and writes the final output to another file—just like the shell's pipe operator.
