import os
import subprocess



gem5_binary = "gem5/build/X86/gem5.opt"
config_script = "gem5/configs/deprecated/example/se.py"

# Define the list of workloads
workloads = [
    "gem5/SPEC.Small/fft/fft",
]

work_options = [

    
]

# Construct the command with the workloads
cmd = ";".join(workloads)  # Join the workloads with semicolons
opt = ";".join(work_options)

option = f'--options="{opt}"'
num_cpus = "-n 1"
cpu_type = "--cpu-type=O3CPU"
l1d_size = "--l1d_size=32kB"
l1i_size = "--l1i_size=32kB"
l2_size = "--l2_size=512kB"
mem_size = "--mem-size=512MB"
l1d_assoc = "--l1d_assoc=2"
l1i_assoc = "--l1i_assoc=2"
l2_assoc = "--l2_assoc=8"
caches = "--caches"
l2cache = "--l2cache"
l1d_rp = "--l1d-rp-type=TreePLRURP"
l1i_rp = "--l1i-rp-type=TreePLRURP"
l2_rp = "--l2-rp-type=TreePLRURP"
l1i_wp = "--l1i-wb-clean=True"
l1d_wp = "--l1d-wb-clean=False"
l2_wp = "--l2-wb-clean=True"

maxTick = "-m 500000000"
warmup = "-W 100000"

#For ruby use following in command
num_dirs = "--num-dirs=16"
mesh_rows = "--mesh-rows=4"
ruby = "--ruby"
network = "--network=garnet"
topology = "--topology=Mesh_XY"
link_latency = "--link-latency=1"
router_latency = "--router-latency=1"

maxTick = "-m 500000000"
warmup = "-W 100000"

# Construct the full command
command = [    
    gem5_binary,
    config_script,
    f'--cmd="{cmd}"',
    # option,
    option,
    num_cpus,
    caches,
    l2cache,
    cpu_type,
    l1d_size,
    l1i_size,
    l2_size,
    mem_size,
    l1d_assoc,
    l1i_assoc,
    l2_assoc,
    l1d_rp,
    l1i_rp,
    l2_rp,
    l1i_wp,
    l1d_wp,
    l2_wp
    # warmup,
    # maxTick,
]

# Run the command
subprocess.run(" ".join(command), shell=True)
