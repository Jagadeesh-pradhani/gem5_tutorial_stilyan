# gem5 Directory Structure

This document provides an overview of the gem5 directory structure to help you navigate and understand its components.

## Table of Contents
- [SIMULATION IN CODESPACE](#SIMULATION-IN-CODESPACE)
- [Top-Level Directories](#top-level-directories)
- [Files Summary](#files-summary)
- [gem5 Simulation Results Analysis Guide](#gem5-simulation-results-analysis-guide)
- [Reults Summary](#results-summary)


---

## **SIMULATION IN CODESPACE**

**Step-1**: Fork this repo and create a new fork in your github. <br>
![image](https://github.com/user-attachments/assets/90a72eea-5c93-43a4-9c8d-8e9965e2e72d)
<br>

**Step-2**: Open the fork and click on `code` <br>
![image](https://github.com/user-attachments/assets/fad4da55-6e43-4b9d-9fb8-377b586669ef)
<br>

**Step-3**: Click on codespaces -> 3 dots -> New with Options <br> 
![image](https://github.com/user-attachments/assets/3879e114-d8f8-4fb0-ba70-ccc97cb1deca)
<br>

**Step-4**: Make sure the selected ooptions include 4-core, and then click on create codespace <br>
![image](https://github.com/user-attachments/assets/638c669a-3e04-4c1e-92c2-32e0235973e0)
<br>

This will open a codespace in github <br>
![image](https://github.com/user-attachments/assets/6d81ece1-9798-455b-a87c-30b24a284c1c)
<br>

Lets set up gem5! <br>
- Install dependencies in the terminal, <br>
(`cd gem5` or `cd /workspaces/gem5_tutorial_stilyan/gem5/`)
```
cd gem5
sudo apt update
sudo apt install python3-pip
pip3 install -r requirements.txt
sudo apt install m4
pip3 install scons
```
- Build GEM5 <br>
 This building process will take around 20-30 mins
```
cd gem5
scons build/X86/gem5.opt -j$(nproc)
```

- RUN SIMULATION <br>
1. Hello world with default configurations
```
cd gem5
build/X86/gem5.opt configs/deprecated/example/se.py -c tests/test-progs/hello/bin/x86/linux/hello
```
2. Custom configuration script
```
cd gem5
build/X86/gem5.opt configs/basics/simple.py 
```

3. Custom script with multicore
```
cd gem5
build/X86/gem5.opt configs/basics/tcmp.py
```

4. Simulation with custom paramters <br>
   Modify the python code to set different parameters
```
cd gem5
python3 run_simulation.py
```



## **Top Level Directories**

### `configs`
- **Purpose**: Contains Python scripts for configuring and running simulations.
- **Subdirectories**:
  - `common`: Utility scripts shared across configurations.
  - `example`: Predefined configuration scripts for common setups (e.g., full-system, syscall emulation).

---

### `ext`
- **Purpose**: External libraries and tools used by gem5.
- **Examples**: Includes `pybind11` and other third-party dependencies.

---

### `src`
- **Purpose**: Main source code for the simulator, organized into subdirectories for various components.
- **Key Subdirectories**:
  - `arch`: Architecture-specific code (e.g., x86, ARM, RISC-V).
  - `base`: Core utilities and shared classes.
  - `cpu`: CPU models (e.g., simple, out-of-order).
  - `mem`: Memory system code (e.g., caches, memory controllers).
  - `dev`: Device-related code (e.g., UART, NIC).
  - `sim`: Simulation infrastructure (e.g., initialization, events).
  - `kern`: Kernel-level interaction code.

---

### `build`
- **Purpose**: Build output files, dependent on the target architecture.
- **Contents**:
  - Compiled binaries (`gem5.opt`, `gem5.debug`).
  - Object files and intermediate build artifacts.

---

### `tests`
- **Purpose**: Scripts and configurations for testing gem5 functionality.
- **Contents**:
  - Unit tests for specific components.
  - Regression tests for full-system simulations.

---

### `util`
- **Purpose**: Utility scripts and tools for working with gem5.
- **Examples**:
  - Trace file conversion.
  - Visualization of simulation outputs.
  - Debugging tools.

---

### `docs`
- **Purpose**: Documentation files in reStructuredText format.
- **Contents**:
  - Tutorials.
  - Component explanations.
  - Contribution guides.

---

### `system`
- **Purpose**: Files needed for full-system simulation.
- **Contents**:
  - `bios`: Firmware files.
  - `disk`: Disk image files.

---

### `python`
- **Purpose**: Python modules for configuring and running simulations.
- **Key Modules**:
  - Simulation parameters.
  - Workload definitions.
  - Cache and memory system configurations.

---

### `m5`
- **Purpose**: Host utilities for interacting with the simulation environment.
- **Examples**:
  - Scripts for debugging and managing simulations.

---

## **Files Summary**

| Directory         | Purpose                                         |
|--------------------|-------------------------------------------------|
| `configs`          | Simulation configuration scripts               |
| `ext`              | External libraries and tools                   |
| `src`              | Main source code for the simulator             |
| `build`            | Compiled binaries and build artifacts          |
| `tests`            | Unit and regression testing                    |
| `util`             | Helper scripts and utilities                   |
| `docs`             | Documentation files for gem5                   |
| `system`           | System-level files (BIOS, disk images, etc.)   |
| `python`           | Python modules for simulation configuration    |

---





## gem5 Simulation Results Analysis Guide

After running simulations, gem5 produces output files in various formats. Here’s how to analyze them:

### 1. `stats.txt`
- **Purpose**: Contains detailed statistics about the simulation.
- **Analysis**:
  - Use a text editor or parse it programmatically (e.g., with Python).
  - Look for specific metrics like IPC, cache hits/misses, execution time, etc.
  - Example tools: Custom Python scripts, `grep`/`awk` for specific metrics.
- **Sample Entry**:
  ```plaintext
  system.cpu.numCycles                       12345678                      # Number of CPU cycles
  ```

### 2. `config.ini`
- **Purpose**: Stores the system configuration used for the simulation.
- **Analysis**:
  - Provides a snapshot of the simulated system (CPU type, cache sizes, etc.).
  - Useful for reproducing simulations or verifying configuration details.
- **Sample Section**:
  ```ini
  [system.cpu]
  type=DerivO3CPU
  numThreads=1
  clock=2GHz
  ```

### 3. `config.json`
- **Purpose**: A JSON version of the system configuration.
- **Analysis**:
  - Easier to parse programmatically compared to `config.ini`.
  - Load using any JSON-compatible library (e.g., Python's json module).
- **Sample Command**:
  ```python
  import json
  with open('config.json', 'r') as f:
      config = json.load(f)
      print(config['system']['cpu']['type'])
  ```

### 4. `simout`
- **Purpose**: The standard output of the simulation.
- **Analysis**:
  - Useful for debugging runtime issues or observing simulation progress.
  - Includes events like cache misses, exceptions, or warnings.

### 5. `simerr`
- **Purpose**: The standard error output of the simulation.
- **Analysis**:
  - Contains error messages or warnings during the simulation.
  - Essential for troubleshooting failed simulations.



## Results Summary

| File         | Purpose                                                 |
|--------------|---------------------------------------------------------|
| `stats.txt`  | Simulation statistics (performance metrics)             |
| `config.ini` | System configuration in INI format                     |
| `config.json`| System configuration in JSON format                    |
| `simout`     | Standard output during simulation                      |
| `simerr`     | Standard error during simulation                       |






