# Paper idea

## Possible Title

**Generation of complex synthetic road networks using a simplified logical description**

## Rough content

* Simple description of road geometry, junctions and connections based on OpenDRIVE
* All road segments are defined in local coordinate systems and are connected relative to each other
* The logical (xml-) language is designed such that variations of road geometry and junction appearance are easy to implement. Meaning that redudancies are elimanated in contrast to classic concrete road description languages such as OpenDRIVE
* With the logical concept comes a tool that calculates missing information to generate a concrete road network written in a standardized language. In this case OpenDRIVE
* Important "scientific" working packages were/are
  * Concept of the logical description, based on disadvantages of OpenDRIVE
    * ODR is highly redundant, therefor it is not easy to adjust single quantities (e.g. one curvature value)
    * clustering of often used variations e.g.: Measures and properties of main/access roads (or at least the possibility to define those)
    * Simple descriptions of T-Junctions and X-Junctions (for the beginning)
    * local coordinates which can be set in relation to one another
  * Generation/Calculation of missing information due to the logical "character". E.g. 
    * the min./max. radius of a right turn in a junction
    * Lane widths inside junctions
    * Meaningful curvature boundaries
    * --> Mostly those quantities are taken from the official regulations of Germany (Richtline für die Anlage von ...)
  * Demonstrating that redundancies of OpenDRIVE are eliminated e.g. by:
    * Changing curvature of a curve --> all road segments still align correctly to each other
    * Adding a lane inside a junction --> only one line is changed in input, OpenDRIVE changes are much more complex
    * ... maybe 1 or 2 more
  * Validation that output is valid OpenDRIVE by:
    * checking the xsd scheme
    * Importing the road network in two commercial tools: CarMaker and VTD
  * If desired, the road network may be closed (to drive in a circle) which is not trivial
    * small optimization problem solved to fulfill the continous curvature criterion

# For Motivation/Introduction

* Reference to 6 Layer Model (EN). Discuss that projects such as PEGASUS focus on Layer 4. However, the static part of the scenario (in this case 1 and 2) may also be described in a simple, logical and most important variable way. 
* At the same time there has to be a mechanism to create concrete (standardized) instances of the description to perform simulations to validate automated driving functions. 
  * C++ tool 
  * open source (or soon to be)
