Input file format
=================

This documentation shows the most common use cases for the Road Generation library. The general outline of our input format consists of a *segments* element, a *links* and a *closeRoad* element.

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

The *segments* element contains information about each physical road in the network. For each seperate road, roundabout or junction, a corresponding child element is added to *segments*.

.. figure:: dias/segments.svg
   :width: 50%
   :align: center

Segments
--------

ConnectingRoad
''''''''''''''

A connecting Road element is used to represent a single street.

.. figure:: dias/connectingroad.svg
   :width: 45%
   :align: center




^^^^

**road**

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    id , int , positive , id of the road inside of the segment, yes
    classification , string , 'main' 'access' , set the road to main or access road , yes



^^^^^^^^

**lineType**

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    length , double , positive , length of the reference line in m, yes
    R , double , positive , constant radius of the arc , yes
    Rs , double , positive , starting radius of the spiral , yes
    Re , double , positive , ending radius of the spiral , yes

Junction
''''''''''''''

A junction must be used if more than two roads are linked to each other.

.. figure:: dias/junction.svg
   :width: 55%
   :align: center



^^^^

**junction**

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    id , int , positive , id of the road inside of the segment, yes
    type , string , '2M' 'MA' 'M2A' '3A', type of junction , yes



^^^^^^^^

**automaticWidening**

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    active , string , 'none' 'all' 'main' 'access' , specify where automatic Widening is applied, yes
    length , double , positive , length of the additional lane, no
    double , double , positive , length of the transitioning part, no

* starting point is always at road beginning (s=0)

**intersectionPoint**

Stores information about the point where all roads intersect. The reference road dictates the position of the junction.

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    refRoad, int , positive , id of the reference road of the junction´, yes
    s , string , positive, position of the junction , yes


**roadLink**

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    fromId, int , positive , id of the fromRoad, yes
    toId, int , positive , id of the toRoad, yes
    fromPos , string , 'start' 'end', position of the fromRoad link , yes
    toPos , string , 'start' 'end', position of the toRoad link , yes


**laneLink**

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    fromId, int , positive , id of the fromLane , yes
    toId, int , positive , id of the toLane, yes
    left , string , 'solid' 'broken' 'none', lane marking on the left side, no
    right , string , 'solid' 'broken' 'none', lane marking on the right side, no










