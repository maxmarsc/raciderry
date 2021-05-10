Master status : ![example branch parameter](https://github.com//maxmarsc/raciderry/actions/workflows/unittests.yml/badge.svg?branch=master)  
Develop status : ![example branch parameter](https://github.com//maxmarsc/raciderry/actions/workflows/unittests.yml/badge.svg?branch=develop)
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
 - [x] Dynamic detection of midi controllers
 - [x] Presets Save & Load
 - [X] Read parameters from an JSON file
 - [x] Automated tests
 - [ ] Write-and-go iso image for rapsberry pi
 - [ ] Presentation Video
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
 - JUCE Framework >= 6.0.8

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

## Testing
The sources are partially tested (should be completed in the future). To manually
run the tests run :
```shell
make -C ./Builds/LinuxMakefile CONFIG=UnitTests
./Builds/LinuxMakefile/build/tests
```

## Run
If configured as recommended, the synth should start at the same time as the 
Raspberry. If not, simply ssh into the Raspberry and run `~/raciderry`.

## Configuration
You can either use the default configuration or override any parameter by writing 
into the `/etc/raciderry.json` configuration file on the raciderry file system.

The default configuration file is `Resources/default_parameters.json`. To override 
a parameter, add its field from the default configuration, set your values,
and add it to the `/etc/raciderry.json` configuration file.

### Audio
Default settings are 192kHz / 64 frames. Which should be enough to reach the
announced 2ms latency of the Pisound (not measured yet).

### Controls
Raciderry is controllable through MIDI signals. It will link to any plugged midi
interface. Controles are customizable (see `Configuration`)

#### Controller messages
Raciderry will only read controller message from its own MIDI channel (parameter
`GLOBAL_CHANNEL`). Each parameter is assigned to a CC control and uses relative
control values :
 - 64 : the parameter keeps the same value
 - 58 -> 63 : lower the parameter value (58 the fastest)
 - 65 -> 70 : increase the parameter value (70 the fastest)

---
Default MIDI assignements : 
|         | ATTACK | DECAY | SUSTAIN | RELEASE | ACCENT | ACCENT_DECAY | WAVEFORM_RATIO | GLIDE | CUTOFF | RESONANCE | FILTER_MIX | ENV_MOD |
|---------|--------|-------|---------|---------|--------|--------------|----------------|-------|--------|-----------|------------|---------|
| CC      | 73     | 75    | 64      | 72      | 83     | 82           | 80             | 81    | 16     | 17        | 18         | 18      |
| CHANNEL | 2      | 2     | 2       | 2       | 2      | 2            | 2              | 2     | 2      | 2         | 2          | 2       |

*customizable, see `Configuration`*

#### Patchs (Save/Load)
The raciderry can save and load patchs into/from a `presets.xml` file.

To load a patch simply send a program change MIDI message with the number of
the patch you wanna load as a the program change value.

To save a patch, first send a CC message (default to CC 20). The raciderry
will then wait for a program change message and will store the current state as
the patch for this value.

## Licensing & third-parties
This project is licensed under the BSD 3-Clause License.

This project uses work from two other projects (filters) : 
 - Open 303 : https://github.com/maddanio/open303
 - MoogLadders : https://github.com/ddiakopoulos/MoogLadders

## Contributions
Contributions are very welcome, especially concerning the dsp.


As the project is very new, please take contact at maxime.coutant@protonmail.com
or on discord at Groumpf#2353

