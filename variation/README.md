#Variation Tool

The Variation tool is used to (randomly) generate a multitude of slightly different scenarios based on the same general road network. A template file is provided to specify all variables aswell as the general road network structure. 

##Requirements
The following modules need to be installed:

- `ElementTree`
- `sympy`

##Usage
The tool can be accessed via the command line:

	$python3 variation -fname <filename> 
	

######A variety of parameters and flaggs can be specified:

- `fname ` `<input file path>` input template file
- `-n` `<integer>`number of generated outputs
- `-h --help` display the help message
- `-k` keep the inetmediate `.xml` output files


##Template File

The template file is structured similar to a `road-generation`input file with the addition of the ` <vars> ... </vars>` tag, which cointains variables that will be generated.


```xml
<vars>
	<var id="transDepVar" type="lindep" dp="depVar/2" /> 
	<var id="depVar" type="lindep" dp="uniformVar*0.3 + normalVar" />        
	<var id="uniformVar" type="uniform" min="100" max="150" />        
	<var id="normalVar" type="normal" mu="1.57" sd="0.12" />     
</vars>
```
These variables can then be referenced by placeholder values. We would reference the `uniformVar` like this: 
```xml
<referenceLine>
	<line length="${uniformVar}"/>
</referenceLine>
```


A sample template file can be found in the `data` directory.


##Variable Types
For each variable we need to specify an `id` and `type`attribute.  At the moment there are three types of variables that require different attributes:

- `type = normal`  drawn from a normal distribution  with mean value `mu` and standard deviation `sd`.
- `type = uniform`  drawn from an uniform distribution with a `min` and a `max` value.
- `type = lindep`  that is linear dependent on other variables or constants with  a dependency equation `dp` in string form.



##Issues
This is small list of known issues with the variation tool in no particular order:

- Variation tool does not check if a referenced variable exists
- Performance gets poor on larger files and high variation counts
- A lot of unnecessary console output






	
