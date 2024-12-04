#Open codespace
#Install following dependencies

sudo apt update
sudo apt install python3-pip
pip3 install -r requirements.txt
sudo apt install m4
pip3 install scons


#Build
scons build/X86/gem5.opt -j$(nproc)

#Run
python3 run_simulation.py