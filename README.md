# Road Generation Tool for Basic OpenDRIVE Road Networks

![Build](https://github.com/ika-rwth-aachen/RoadGeneration/actions/workflows/build.yml/badge.svg?branch=main) ![MIT](https://img.shields.io/badge/license-MIT-blue.svg)  
``#road`` ``#openDRIVE`` ``#generic`` ``#generation``

<!--<div align="center">
    <img src="/doc/logo.jpg" width="350px"</img> 
</div>-->

## About

Simulation is a valuable building block for the verification and validation of automated driving functions (ADF). When simulating urban driving scenarios, simulation maps are one important component. Often, the generation of those road networks is a time consuming and manual effort. Furthermore, typically many variations of a distinct junction or road section are demanded to ensure that an ADF can be validated in the process of releasing those functions to the public.  

Therefore, we present a prototypical solution for a logical road network description which is easy to maintain and modify. The concept aims to be non-redundant so that changes of distinct quantities do not affect other places in the code and thus the variation of maps is straightforward. In addition, the simple definition of junctions is a focus of the work. Intersecting roads are defined separately and then set in relation, finally a junction is generated automatically.  

The idea is to derive the description from a commonly used, standardized format for simulation maps in order to generate this format from the introduced logical description. Consequently, we developed a command-line tool that generates the standardized simulation map format OpenDRIVE. Furthermore, there exists a Python package that allows the user to introduce stochastic variables for each quantity and generate as many variations of a logically identical road network as desired.

## Overview

This repository provides a tool for the generation of road networkds. Here, the main folders are named:

- `io`: contains sample input files
- `src`: contains the source code
- `test`: contains test files and their desired OpenDRIVE outputs
- `xml`: contains the xsd validation files
- `variation`: contains the variation tool

## Installation

The following **requirements** have to be satisfied:

- ``C++11``
- ``CMake 2.6 or higher``
- ``Python 3.6``
- ``xercesC``

**Download** the repository as a zip-file and un-zip, or use git with

```bash
    # Clone Repository and open main folder
    git clone git@github.com:ika-rwth-aachen/RoadGeneration.git
    cd RoadGeneration
```

A build script is provided and can be executed from the root directory with

```bash
sh buildScript.sh
```

Alternatively you can **build the project manually**:

<details><summary><b>Build on Linux</b></summary>

1. Install [`xercesC`](https://xerces.apache.org/xerces-c/)

    You can use a packet manager
    ```bash
    sudo apt install libxerces-c-dev
    ```
    Or download the source directly
    ```bash
    curl https://mirrors.gigenet.com/apache//xerces/c/3/sources/xerces-c-3.2.3.tar.gz --output xerces-c-3.2.3.tar.gz
    ```
    and unpack it with
    ```bash
    gzip -d xerces-c-3.2.3.tar.gz
    tar -xf xerces-c-3.2.3.tar
    ```
    Finally build the source files as instructed [here](https://xerces.apache.org/xerces-c/build-3.html).

2. Build the Road-generation tool
```bash
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../bin ..
    cmake --build .
```
</details>


<details><summary><b>Build on Windows</b></summary>

THIS IS SUBJECT TO CHANGE
1. Install [`xercesC`](https://xerces.apache.org/xerces-c/)

    You can use a packet manager
    ```bash
    sudo apt install libxerces-c-dev
    ```
    Or download the source directly
    ```bash
    curl https://mirrors.gigenet.com/apache//xerces/c/3/sources/xerces-c-3.2.3.tar.gz --output xerces-c-3.2.3.tar.gz
    ```
    and unpack it with
    ```bash
    gzip -d xerces-c-3.2.3.tar.gz
    tar -xf xerces-c-3.2.3.tar
    ```
    Finally build the source files as instructed [here](https://xerces.apache.org/xerces-c/build-3.html).

2. Build the Road-generation tool
```bash
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../bin ..
    cmake --build .
```
</details>

## Usage

The compiled application can be called from the root folder:

```bash
    ./road-generation_executable <input>
```

This generates the output OpenDRIVE file next to the input file. The provided input file is checked against ``input.xsd``. Analogous the output file is checked against the ``output.xsd`` file which specifies the openDRIVE 1.5 standard. For a list of all parameters use the help flag of the tool.

```bash
    ./road-generation_executable -h
```

## Documentation

A simple but well designed C++ reference [documentation](https://ika-rwth-aachen.github.io/RoadGeneration/index.html) is provided.

*TODO: take care of XSD input file documenation.*

## Variation tool

The Variation tool is used to generate a variety of different scenarios based on the same general road network. A template file is provided to specify variables as well as the general road network structure. A more thorough documentation can be found in the [variation subdirectory](variation/README.md).

## Licenses

The project is distributed under the [MIT License](LICENSE.md).

## Contact

Daniel Becker
| Institute for Automotive Engineering (ika)
| RWTH Aachen University
| daniel.becker@ika.rwth-aachen.de
