Input file format
=================

This documentation shows the most common use cases for the library.

.. code-block:: xml

    <roadNetwork xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="../xml/input.xsd">
    	<segments>
            ...
    	</segments>
        <links refId="1" hdgOffset="0.0" xOffset="0" yOffset="0">
            ...
        </links>
        <closeRoads>
            ...
        </closeRoads>
    </roadNetwork>


.. figure:: dias/segments.svg
   :width: 50%
   :align: center

Segments
--------

The *segments* element contains information about each physical road in the network. For each seperate road, roundabout or junction a corresponding child element should be added to *segments*.

ConnectingRoad
''''''''''''''

A connecting Road element is used to represent a single street.

.. figure:: dias/connectingroad.svg
   :width: 45%
   :align: center





**road**
^^^^

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    id , int , positive , id of the road inside of the segment, yes
    classification , string , 'main' 'access' , set the road to main or access road , yes


**lineType**
^^^^^^^^

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    length , double , positive , length of the reference line in m, yes
    R , double , positive , constant radius of the arc , yes
    Rs , double , positive , starting radius of the spiral , yes
    Re , double , positive , ending radius of the spiral , yes


**Key points to consider**
^^^^^^^^

* Connecting roads are used for simple streets that are linked once at each endpoint at most
* The outermost ConnectingRoad element holds the ID in the segment namespace
* The road element holds an ID that is relative to the ConnectingRoad's ID. This is used to distinguish seperate parts of the same segment element
  

Junction
''''''''''''''

.. figure:: dias/junction.svg
   :width: 55%
   :align: center




**junction**
^^^^^^^^^^^

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    id , int , positive , id of the road inside of the segment, yes
    type , string , '2M' 'MA' 'M2A' '3A', type of junction , yes




**automaticWidening**
^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    active , string , 'none' 'all' 'main' 'access' , specify where automatic widening is applied, yes
    length , double , positive , length of the additional lane, no
    double , double , positive , length of the transitioning part, no


**intersectionPoint**
^^^^^^^^^^^^^^^^^^^^^

The intersection point stores information about the location and geometry about the point in which all roads meet. The reference road dictates the position of the junction.

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    refRoad, int , positive , id of the reference road of the junction, yes
    s , string , positive, position of the junction in road direction , yes


**RoadLink**
^^^^^^^^^^^^

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    fromId, int , positive , id of the fromRoad, yes
    toId, int , positive , id of the toRoad, yes
    fromPos , string , 'start' 'end', position of the fromRoad link , yes
    toPos , string , 'start' 'end', position of the toRoad link , yes


**laneLink**
^^^^^^^^^^^^

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    fromId, int , positive , id of the fromLane , yes
    toId, int , positive , id of the toLane, yes
    left , string , 'solid' 'broken' 'none', lane marking on the left side, no
    right , string , 'solid' 'broken' 'none', lane marking on the right side, no

**coupler**
^^^^^^^^^^^

Holds detailed junction properties about the linkage of the lanes and the junction area.


Key points to consider:
^^^^^^^^^^^^^^^^^^^^^^

* Junctions need to be linked to the starting point of each adjacent road
* A junction must be used if more than two roads are linked to each other
* The intersection points of access roads need to be linked at their start or end positions
* Similar to the connecting road namespace, the junction ID is in the segment ID namespace and the road IDs are relative to the segment ID
  


**Example**
^^^^^^^^^^^

.. code-block:: xml

    <roadNetwork xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="../xml/input.xsd">
        ...
            <junction id="1" type="M2A">
                <road id="1" classification="main" >	
                    <referenceLine>
                        <line length="200"/>
                    </referenceLine>
                </road>
                <road id="2" classification="access" >	
                    <referenceLine>
                        <line length="100"/>
                    </referenceLine>
                </road>
                <road id="3" classification="access" >	
                    <referenceLine>
                        <line length="100"/>
                    </referenceLine>
                </road>
                <intersectionPoint refRoad="1" s="100">
                    <adRoad id="2" s="20" angle="-1.57"/>
                    <adRoad id="3" s="20" angle="1.57"/>
                </intersectionPoint>
                <coupler>
                    <junctionArea gap="10">
                        <roadGap id="2" gap="15"/>
                        <roadGap id="3" gap="15"/>
                    </junctionArea>
                </coupler>
            </junction>
        ...
    </roadNetwork>


Roundabout
''''''''''
Represents a simple roundabout. The tool will then generate several junctions belonging to a junction group in the output.

.. figure:: dias/roundabout.svg
   :width: 55%
   :align: center

**Circle**
^^^^^^^^^^^
.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    id, int , positive , id of the circle (usually 1) , yes
    classification, int , 'main' 'access' , sets the circular road of the rb to access or main road, yes


**Reference Line**
^^^^^^^^^^^
This tag contains another circle that stores the length of the reference line. *Note:* This circle tag is semantically and syntactically different from the circle node above.

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    length, double , positive , length of the circular road in the roundabout , yes


**Example**
^^^^^^^^^^^

.. code-block:: xml

   <roadNetwork xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="../xml/input.xsd">
    ...

      <roundabout id="1">
            <circle id="1" classification="access" >	
                <referenceLine>
                    <circle length="320.5"/>
                </referenceLine>
                <lanes>
                        <lane id="1" type="delete" />
                       
                        <lane id="-3" type="driving">
                            <roadMark   type="solid" color="white" width="0.13"/>
                        </lane>
                        <lane id="-2" type="driving">
                            <roadMark   type="broken" color="white" width="0.13"/>
                        </lane>
                        <lane id="-1" type="driving">
                            <roadMark   type="broken" color="white" width="0.13"/>
                        </lane>
                        <lane id="0" type="driving" />
                </lanes>
            </circle>
            
            <road id="2" classification="access" >	
                <referenceLine>
                     <line length="150"/>
                </referenceLine>
            </road>
            <road id="3" classification="access" >	
                <referenceLine>
                     <line length="100"/>
                </referenceLine>
            </road>
            <road id="4" classification="access" >	
                <referenceLine>
                     <line length="100"/>
                </referenceLine>
            </road>
            <road id="5" classification="access" >	
                <referenceLine>
                     <line length="100"/>
                </referenceLine>
            </road>
            
            <intersectionPoint refRoad="1" s="80">
                <adRoad id="2" s="20" angle="-1.56"/>
            </intersectionPoint>
            <intersectionPoint refRoad="1" s="160">
                <adRoad id="3" s="20" angle="-1.56"/>
            </intersectionPoint>
            <intersectionPoint refRoad="1" s="240">
                <adRoad id="4" s="20" angle="-1.56"/>
            </intersectionPoint>
            <intersectionPoint refRoad="1" s="320">
                <adRoad id="5" s="20" angle="-1.6"/>
            </intersectionPoint>
            <coupler>
                <junctionArea gap="20">
                </junctionArea> 
            </coupler>
        </roundabout>
    ...
    </roadNetwork>


Lanes
-------

Linkage
-------


Closing the Network
----------------



