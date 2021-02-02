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

The synthesizer offers OSC based control over all settings and performance parameters. Any tool can thus be used to control the synthesizer. The [BinBong]((https://gitlab.tubit.tu-berlin.de/henrikvoncoler/binbong_2)) is a haptic control interface, designed
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

	$ git clone https://gitlab.tubit.tu-berlin.de/henrikvoncoler/GLOOO_synth.git


**Install dependencies:**

```shell
$ sudo apt-get install libjack0
$ sudo apt-get install libjack-jackd2-dev
$ sudo apt-get install libyaml-cpp-dev
$ sudo apt-get install libboost-dev
$ sudo apt-get install liblo-dev
```

 

**Make sure cmake is installed:**

    $ sudo apt-get install cmake

- requires CMake > 3.10

**In the root directory of the repository run:**

```shell
$ mkdir build
$ cd build
$ cmake ../.
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
    $ ./GLOOO_synth -m ../MODELS/Violin/ -c ../setup/glooo_settings.yml

The synth takes a considerable time for reading all model data.
Once loaded, it keeps running, waiting for OSC control input.


------

## 2: Periphery

### Spatialization

The GLOOO_synth can be used without any further tools when
provided with the proper OSC commands.
The full *GLOOO instrument* relies on additional spatial
rendering software integrated as another Jack client.
Initially, [PanoramixApp](https://forum.ircam.fr/projects/detail/panoramix/) was used and is still supported.
[SC_SPAT](https://gitlab.tubit.tu-berlin.de/henrikvoncoler/sc_spat/) for spatialization of the individual partials.

When using SC_SPAT, the binaural server can be used
with full 104 sources:

    $ sclang -u 57121 sc_spat_binaural_SERVER.sc 104


### Puredata

[PureData](https://puredata.info/) serves as a control interface and
mapping environment for the GLOOO_synth.
Use deken to install the additional externals:

- `mrpeach` (network stuff)
- `hcs` (pi)




### Qjackctl

Qjackctl can be handy for managing connections:

    $ sudo apt-get install qjackctl



### Interface

The [BinBong](https://gitlab.tubit.tu-berlin.de/henrikvoncoler/binbong_2)
is a haptic musical interface, designed for the use with the GLOOO_synth.
The **Pure Data** control patch is needed to map sensor
data from interface to synthesis and spatialization:

    $ puredata -noaudio -callback -nrt PD/GLOOO_MAIN.pd

-----


## Contributions

* Henrik von Coler [2016-2020]
* Benjamin Wiemann [2016-2017]
* Yrkkö Äkkijyrkkä [2019-2020]
