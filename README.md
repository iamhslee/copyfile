# copyfile
2024-1 Operating Systems (ITP30002) - HW #1

A homework assignment to copy files and directories, just like the `cp` command in Linux, but simpler using C language.

### Author
Hyunseo Lee (22100600) <hslee@handong.ac.kr>

## 1. How to build
This program is written in C language on Ubuntu 22.04.1 LTS. To build this program, you need to use Ubuntu 22.04.1 LTS with GCC and Make installed.

After unarchiving the folder, you should see the following files:
```bash
$ ls
Makefile  README.md  main.c
```

To build the program, run the following command in the terminal:
```bash
$ make
```

To build the program with debug mode, run the following command in the terminal:
```bash
$ make debug
```

To cleanup the build files and test files, run the following command in the terminal:
```bash
$ make clean
```

You can also generate the test files by running the following command in the terminal:
```bash
$ make testenv
```

## 2. Usage
To run the program, run the following command in the terminal:
```bash
$ ./copyfile <Options>
```

copyfile supports the following options:

### 2-1. Copy source file to target file
```bash
$ ./copyfile -f <Source File> <Target File>
```

### 2-2. Copy multiple source files to target directory
```bash
$ ./copyfile -m <Source File 1> <Source File 2> ... <Source File N> <Target Directory>
```

### 2-3. Copy all files and directories from source directory to target directory
```bash
$ ./copyfile -d <Source Directory> <Target Directory>
```

### 2-4. Verbose mode
You can use verbose mode with each option by adding `v` option at the end of the command. For example:
```bash
# Copy source file to target file with verbose mode
$ ./copyfile -fv <Source File> <Target File>

# Copy multiple source files to target directory with verbose mode
$ ./copyfile -mv <Source File 1> <Source File 2> ... <Source File N> <Target Directory>

# Copy all files and directories from source directory to target directory with verbose mode
$ ./copyfile -dv <Source Directory> <Target Directory>
```

You can also see the usage explained above by running the following command in the terminal:
```bash
$ ./copyfile -h
```