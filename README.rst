Road Generation Tool for Basic OpenDRIVE Road Networks
======================================================
.. image:: https://github.com/ika-rwth-aachen/RoadGeneration/actions/workflows/build.yml/badge.svg?branch=main
.. image:: https://camo.githubusercontent.com/83d3746e5881c1867665223424263d8e604df233d0a11aae0813e0414d433943/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f6c6963656e73652d4d49542d626c75652e737667





About
-----

.. inclusion-marker


Simulation is a valuable building block for the verification and
validation of automated driving functions (ADF). When simulating urban
driving scenarios, simulation maps are one important component. Often,
the generation of those road networks is a time consuming and manual
effort. Furthermore, typically many variations of a distinct junction or
road section are demanded to ensure that an ADF can be validated in the
process of releasing those functions to the public.

Therefore, we present a prototypical solution for a logical road network
description which is easy to maintain and modify. The concept aims to be
non-redundant so that changes of distinct quantities do not affect other
places in the code and thus the variation of maps is straightforward. In
addition, the simple definition of junctions is a focus of the work.
Intersecting roads are defined separately and then set in relation,
finally a junction is generated automatically.

The idea is to derive the description from a commonly used, standardized
format for simulation maps in order to generate this format from the
introduced logical description. Consequently, we developed a
command-line tool that generates the standardized simulation map format
`ASAM OpenDRIVE <https://www.asam.net/standards/detail/opendrive/>`__.
Furthermore, there exists a Python package that allows the user to
introduce stochastic variables for each quantity and generate as many
variations of a logically identical road network as desired.

The proposed workflow can be seen in Fig. 1. Further information is
published in `[1] <https://arxiv.org/abs/2006.03403>`__ and
`[2] <https://arxiv.org/abs/2210.00853>`__. This repository exlucdes the
analysis of real world HD maps which is done in a separate internal
tool.

.. image:: docs/_static/motivation.png
   :alt: Fig.1: Possible workflow for the presend road variation tool.

Fig.1: Possible workflow for the presend road variation tool.

Documentation
-------------

If you want to include the library in a project, a simple but well designed C++ reference
`documentation <https://ika-rwth-aachen.github.io/RoadGeneration/index.html>`__ is provided.
The input file format specification can be found `here <https://ika-rwth-aachen.github.io/RoadGeneration/inputdoc.html>`_.

Variation tool
--------------

The Variation tool is used to generate a variety of different scenarios
based on the same general road network. A template file is provided to
specify variables as well as the general road network structure. A more
thorough documentation can be found in the `variation
subdirectory <variation/>`__.



Repository Overview
-------------------

This repository provides a tool for the generation of road networks.
Here, the main folders are named:

-  ``doc``: Resources for documentation
-  ``examples``: Examlpe and Template files that showcase minimal working input networks
-  ``src``: Source code
-  ``test``: Test file .xml and .xodr pairs
-  ``xml``: Contains the XSD validation files
-  ``variation``: Python based variation tool for the road generator

Installation
------------

Road Generation
~~~~~~~~~~~~~~~

The following **requirements** have to be satisfied:

-  ``C++11``
-  ``CMake 2.6 or higher``
-  ``Python 3.6``
-  ``xercesC``

**Download** the repository as a zip-file and un-zip, or use git with

.. code:: bash

 # Clone Repository and open main folder
 git clone git@github.com:ika-rwth-aachen/RoadGeneration.git
 cd RoadGeneration

A build script for Linux systems is provided and can be executed from
the root directory with

.. code:: bash

 sh buildScript.sh

Alternatively you can **build the project manually**:

1. Install `XercesC <https://xerces.apache.org/xerces-c>`_ via a
 package manager, e.g.:

.. code:: bash

  $ sudo apt install libxerces-c-dev

2. Build the Road-generation tool with standard cmake commands, e.g.:

.. code:: bash

  $ mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../bin ..
  $ cmake --build .

*Note:* In principal, it is possible to compile and use the tool in
Windows operating systems. However, this is experimental.

Variation tool
~~~~~~~~~~~~~~

You can run the code from the ``variation`` subfolder, however, the
variation tool can be installed system wide by using the install script
in the root directory.

.. code:: bash

 sh install-variation.sh

Usage
-----

The compiled application can be called from the root folder:

.. code:: bash

   ./road-generation_executable <input>

This generates the output OpenDRIVE file next to the input file. The
provided input file is checked against ``input.xsd``. Analogous the
output file is checked against the ``output.xsd`` file which specifies
the openDRIVE standard. For a list of all parameters use:

.. code:: bash

   ./road-generation_executable -h


Licenses
--------

The project is distributed under the `MIT License <LICENSE.md>`__.

Authors of this Work
--------------------

Scientific Researcher:
~~~~~~~~~~~~~~~~~~~~~~

| `Daniel Becker <https://github.com/dbeckerAC>`__ -
  daniel.becker@ika.rwth-aachen.de
| `Christian Geller <https://github.com/cgeller>`__ -
  christian.geller@ika.rwth-aachen.de
| Fabian Ruß - fabian.russ@ika.rwth-aachen.de

Student Worker:
~~~~~~~~~~~~~~~
`Jannik Busse <https://github.com/jannikbusse>`__

Citation
--------

| We hope our work provides useful help in your research. If this is the
  case, please cite:
| [1]

::

   @INPROCEEDINGS{BeckerRussGeller2020,
     author    = {Daniel Becker and
                  Fabian Ru{\ss} and
                  Christian Geller and
                  Lutz Eckstein},
     title     = {Generation of Complex Road Networks Using a Simplified Logical Description
                  for the Validation of Automated Vehicles},
     booktitle = {2020 IEEE 23rd International Conference on Intelligent Transportation Systems (ITSC)},
     year      = {2020},
     url       = {https://arxiv.org/abs/2006.03403},
     doi       = {10.1109/ITSC45102.2020.9294664}}

[2]

::

   @INPROCEEDINGS{BeckerGeller2022,
     author    = {Daniel Becker and
                  Christian Geller and
                  Lutz Eckstein},
     title     = {Road Network Variation Based on HD Map Analysis for the Simulative Safety  Assurance of Automated Vehicles},
     booktitle = {2022 International Conference on Electrical, Computer, Communications and Mechatronics Engineering (ICECCME)},
     year      = {2022},
     url       = {https://arxiv.org/abs/2210.00853},
     doi       = {10.1109/ICECCME55909.2022.9988312}}

Acknowledgements and Credits
----------------------------

This work received funding from the research project “`SET
Level <https://setlevel.de/>`__” of the
`PEGASUS <https://pegasus-family.de>`__ project family, promoted by the
German Federal Ministry for Economic Affairs and Climate Action based on
a decision of the German Bundestag.

.. |image1| image:: https://setlevel.de/assets/logo-setlevel.svg
   :align: middle
   :width: 100pt
.. _image1: https://setlevel.de/

.. |image2| image:: https://setlevel.de/assets/logo-pegasus-family.svg
   :width: 100pt
   :align: middle
.. _image2: https://pegasus-family.de/

.. |image3| image:: https://setlevel.de/assets/logo-bmwk-en.svg
   :width: 100pt
   :align: middle
.. _image3: https://www.bmwk.de/Redaktion/DE/Textsammlungen/Technologie/fahrzeug-und-systemtechnologien.html


.. list-table::
   :widths: 30 30 30
   :header-rows: 1

   * - SET Level
     - PEGASUS Family
     - BMWK
   * - |image1|_
     - |image2|_
     - |image3|_
  
