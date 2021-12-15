# GLOOO_synth

The GLOOO_synth is a standalone
software for statistical spectral modeling synthesis.
The source code and operating instructions
can be foundin the documentation:

https://hvc.berlin/glooo_synth_doc/index.html

**Spatialization**

GLOOO_synth is designed as a tool for spatial sound synthesis when combined with external rendering software.
[SC_SPAT](https://gitlab.tubit.tu-berlin.de/henrikvoncoler/sc_spat/),
a SuperCollider based framework for different spatialization techniques, is recommended for this purpose.

**Control**

The synthesizer offers basic MIDI control and OSC based control over all settings and performance parameters. Any tool can thus be used to control the synthesizer. The [BinBong]((https://gitlab.tubit.tu-berlin.de/henrikvoncoler/binbong_2)) is a haptic control interface, designed
for the use with GLOOO_synth.

**Analysis & Modeling**

The GLOOO_synt uses a statistical data set for
sound generation. A violin model is included in
the synth repository. It has been created using
the [TU-Note Violin Sample Library](https://depositonce.tu-berlin.de/handle/11303/7527).

Using the Matlab based analysis tool [GLOOO_analysis](https://gitlab.tubit.tu-berlin.de/henrikvoncoler/GLOOO_analysis), new models can be generated.


------

## 1: Build & Run GLOOO_synth

### Compile & Install

The following instructions work for a fresh Ubuntu Studio 20.04 install but should be applicable on most Linux systems.

**Get the sources from the repository:**

	$ git clone https://github.com/anwaldt/GLOOO_synth.git


**Install dependencies:**

```shell
$ sudo apt-get install libjack-jackd2-dev
$ sudo apt-get install libyaml-cpp-dev
$ sudo apt-get install libboost-dev
$ sudo apt-get install liblo-dev
$ sudo apt-get install librtmidi-dev
```

**Make sure cmake is installed:**

    $ sudo apt-get install cmake

- requires CMake > 3.10

**In the root directory of the repository run:**

```shell
$ cd build
$ cmake .
$ make
```

** For a system-wide installation run:**

    $ sudo make install


### Run


The binary needs two arguments:
- the directory of the model to be used.
- the path to the yaml configuration file.

After compilation, the included model and settings file can be used:

    $ cd build
    $ ./GLOOO_synth -m ../MODELS/Violin/ -c ../config/glooo_settings.yml

The synth takes a considerable time for reading all model data.
Once loaded, it keeps running, waiting for OSC or MIDI control input based on the settings.


------

## 2: Periphery

### Qjackctl

Qjackctl can be handy for managing connections:

    $ sudo apt-get install qjackctl

### MIDI & OSC

To control the GLOOO_synth with a MIDI input device, the correct MIDI port has to be chosen. 
The file **_glooo_settings.yml_** has to be changed as follows:
```
####################################################################################################################
# Communication Settings
####################################################################################################################

osc_port_in: 5100          
osc_port_out: 1234        
receive_osc: 0 #<- Disable OSC control 
midi_port_in: 3 #<- Has to be replaced with the correct port number! 
receive_midi: 1 #<- Enable midi 
```
To find the correct port number, run the GLOOO_synth like described above with **_receive_midi: 1_** and the available ports will be listed. 

Supported Control Change messages:
- `CC 48`   (Master volume)
- `CC 49`   (Tonal volume)
- `CC 50`   (Noise volume)

To control the GLOOO_synth with OSC messages, the file **_glooo_settings.yml_** has to be changed as follows:
```
####################################################################################################################
# Communication Settings
####################################################################################################################

osc_port_in: 5100 #<- Has to be replaced with the correct port number!
osc_port_out: 1234 #<- Has to be replaced with the correct port number! 
receive_osc: 1 #<- Enable OSC  
midi_port_in: 3 
receive_midi: 0 #<- Disable MIDI 
```

### Contributions

* Henrik von Coler [2016-2020]
* Benjamin Wiemann [2016-2017]
* Yrkkö Äkkijyrkkä [2019-2020]
