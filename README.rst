Road Generation
===============

Generation of Complex Road Networks Using a Simplified Logical Description for the Validation of Automated Vehicles
--------------------------------------------------------------------------------------------------------------------

|version| |project|


``#road`` ``#openDRIVE`` ``#generic`` ``#generation``

Overview 
--------
This repository provides a tool for the generation of road networkds. Here, the main folders are named:

* `io`_: contains some sample input files
* `doc`_: contains the code and xsd input file documentation and the seminar thesis
* `libs`_: contains the pugixml library
* `src`_: contains the source code
* `xml`_: contains the xsd validation files 

.. _`io`: io
.. _`doc`: doc
.. _`libs`: libs
.. _`src`: src
.. _`xml`: xml

Abstract 
--------
Simulation is a valuable building block for the verification and validation of automated driving functions (ADF). When simulating urban driving scenarios, simulation maps are one important component. Often, the generation of those road networks is a time consuming and manual effort. Furthermore, typically many variations of a distinct junction or road section are demanded to ensure that an ADF can be validated in the process of releasing those functions to the public.
Therefore, we present a prototypical solution for a logical road network description which is easy to maintain and modify. The concept aims to be non-redundant so that changes of distinct quantities do not affect other places in the code and thus the variation of maps is straightforward. In addition, the simple definition of junctions is a focus of the work. Intersecting roads are defined separately, are then set in relation and the junction is finally generated automatically.
The idea is to derive the description from a commonly used, standardized format for simulation maps in order to generate this format from the introduced logical description. Consequently, we developed a command-line tool that generates the standardized simulation map format OpenDRIVE.

Documentation
-------------
A simple but well designed code documentation is provided by using `Doxygen`_

.. _`Doxygen`: http://www.doxygen.nl/


.. code-block:: bash

    cd doc/
    doxygen doxygenDocumentation.doxy
    open doxygenDocumentation/html/index.html

An documentation for the input file of the tool can be generated by the following. It is based on the `xs3p`_ style guide.

.. _`xs3p`: https://xml.fiforms.org/xs3p/

.. code-block:: bash

    cd doc/
    ./xsdDocumentation.sh

Installation
------------

The following requirements have to be satisfied:

- ``C++11``
- ``Python 3.6``
- ``xercesC``
- ...

Download the repository as a zip-file and un-zip, or use git with

.. code-block:: bash

    # Clone Repository and open main folder
    git clone git@git.rwth-aachen.de:dbecker/road-generation.git
    cd road-generation

The installation of xercesC is required and can be conducted by:

.. code-block:: bash

    # Install xercesC
    ./build_xercesc_win64

The actual source code can be compiled with the familiar cmake procedure.

.. code-block:: bash

    # Compile
    cmake .
    make

Tests
-----

A CI pipeline is provided by default, but an own ``gitlab-runner`` has to be created and added inside of ``gitlab``. 

The test evironment checks an successful build, and a correct output of given input files after each commit automatically. The generated output files are compared with given reference data.

Getting Started
---------------

For usage the compiled application can be called from the ``PROJ_DIR`` folder:

.. code-block:: bash

    ./roadGeneration <input>.xml

This generates the output XML file in the folder of the input file. The provided input file is checked against the ``input.xsd`` file. Please see this file for specific information about the input format. Analogous the output file is checked against the ``output.xsd`` file specifying the current openDRIVE standard.

The file all.sh provides a generation of all sample input files in `io`_.

Variation of Parameters
-----------------------

A simple variation python-based tool is provided in variation. This tool generated input files from a given template by disturbing specific parameters in a defined manner. The input files are translated to the OpenDRIVE output in the familiar way. 

Notes
-----

A simple, but sufficient visualization can be processed by using the OpenDrive Viewer, maintained by VIRES. Other visualization tools are CarMaker oder Roadrunner.

Here are some basic informations about the input format:

* segments: tjunction, xjunction, roundabout or connectingRoad
* type for an X-junction can be either 2M, 4A, M2A 

Further Work
------------

* specific road markings
* documentation for the input file
  
Contact
-------
:Author:
    | Christian Geller
    | christian.geller@rwth-aachen.de
:Supervisor:
    | Daniel Becker
    | Institute for Automotive Engineering (ika)
    | RWTH Aachen University
    | daniel.becker@ika.rwth-aachen.de

.. |version| image:: https://img.shields.io/badge/version-0.2-blue.svg
    :target: https://gitlab.ika.rwth-aachen.de/dbecker/road-generation
    :alt: Documentation Website

.. |project| image:: https://img.shields.io/badge/project-RoadGeneration-blue.svg
    :target: https://gitlab.ika.rwth-aachen.de/dbecker/road-generation
    :alt: Documentation Website