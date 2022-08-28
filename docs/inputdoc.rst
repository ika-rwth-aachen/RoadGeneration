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
   :width: 30%
   :align: center
