# Raciderry

## Brief
Raciderry is a digital acid synthesiser, controlled by MIDI inputs, and designed
to run on a Raspberry Pi with a Pisound extension.

**Please note this project is still under active development and should evolve
quickly**

### Where we at ?
 - [x] Waveform control
 - [x] Ladder filter + controls
 - [x] ASDR amplitude envelope + controls + filter modulation
 - [x] Accent's envelope to modulate filter + controls
 - [x] Noise on each parameter to get a more vintage sound
 - [x] Wavetable synthesis to get more authentics waveforms
 - [ ] FM modulation + controls
 - [x] Dynamic detection of midi controllers
 - [ ] Read parameters from an XML file
 - [ ] Automated tests
 - [ ] Write-and-go iso image for rapsberry pi
 - [x] Documentation
 - [x] Readme(to be improved)

## Requirements
Synthesiser requirements:
 - Raspberry Pi 3B+ (not tested on other models, volunteers are welcome)
 - Pisound extension : https://blokas.io/pisound/

Build requirements :
 - Linux
 - Docker
 - qemu-arm-static

---

Developping requirements (recommended):
 - *All requirements listed above*
 - JUCE Framework >= 6.0.1

## Build and deploy

### Preconfiguration steps for the Raspberry Pi
1. For now, raciderry should run on the patchbox OS provided by Blokas. See this 
link for download and installation instructions :  
https://blokas.io/patchbox-os/

*Additional modules installation provided by the patchbox wizard script are NOT
required*

2. Activate `ssh` on the patchbox system by creating an empty `.ssh` file at
the root of the boot system

3. Make sure your system is reachable at `patch@patchbox.local`


### Build
#### Option 1 : emulated native compilation
To ease the development process, we choose to use emulated native compilation
using Docker and qemu. 


*[Only once]* Install  and configure qemu
```shell
sudo apt-get install qemu-user-static
cp /usr/bin/qemu-arm-static EmulatedCompiler/
sudo mount binfmt_misc -t binfmt_misc /proc/sys/fs/binfmt_misc
echo ':arm:M::\x7fELF\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x28\x00:\xff\xff\xff\xff\xff\xff\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff\xff:/usr/bin/qemu-arm-static:' > /proc/sys/fs/binfmt_misc/register
```

Build : ***This currenlty builds the debug executable***
```shell
./Scripts/build_and_deploy.sh
```

The raciderry executable should now be copied to `home/patchbox` in your 
Raspberry file system

#### Option 2 : TBA

### Configuration
This step is needed to make sure the raciderry executable starts at startup and 
with the proper parameters

First, on your building host (probably x86_64 Linux), run :
```shell
./Scripts/configure_remote_install.sh
```

Then, ssh at `~` on your raspberry and run :
```shell
sudo ./install.sh
```

You should be good !


## Run
If configured as recommended, the synth should start at the same time as the 
Raspberry. If not, simply ssh into the Raspberry and run `~/raciderry`.

## Configuration
All the parameters can be found in the header : `Source/Utils/Parameters.h`

*Should change for an XML file at some point*

### Audio
Default settings are 96000kHz with 256 buffer size => ~5ms latency

### Controls
TODO: table with MIDI bindings when I'll stop changing midi assignments every
two days

---
## Contributions
Contributions are very welcome, especially concerning the dsp.


As the project is very new, please take contact at maxime.coutant@protonmail.com
or on discord at Groumpf#2353

