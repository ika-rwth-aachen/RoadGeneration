# requires an XSLT tool such as xsltproc (preinstalled on Mac OS)

xsltproc --stringparam title "Road Generation Input Documentation" xs3p_2.xsl ../xml/input.xsd > xsdDocumentation.html