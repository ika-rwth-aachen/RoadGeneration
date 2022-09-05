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

The *segments* element contains information about each physical road represented in the network. For each seperate road, a corresponding child element is added to *segments*.

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
    classification , string , 'main' 'access' , decides if the road is a main or access road , yes



^^^^^^^^

**lineType**

.. csv-table::
    :widths: 50 50 50 50 50

    **Name** , **Type** , **Range** , **Description** , **Required**
    length , double , positive , length of the reference line in m, yes
    R , double , positive , constant radius of the arc , yes
    Rs , double , positive , starting radius of the spiral , yes
    Re , double , positive , ending radius of the spiral , yes



