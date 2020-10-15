##Variation Tool

The Variation Tool is used to (randomly) generate a multitude of slightly different scenarios based on the same general road network. A template file is provided to specify all variables aswell as the general road network structure. 

###Usage
The tool can be accessed via the command line:

	$python3 variation -fname <filename> 
	
#####Parameters:
A variety of parameters or flaggs can be specified:

- `fname`The input template file
- `-h --h` Display the help message
- `-k` Keep the inetmediate xml output files
- `-n` number of generated outputs

###Template file
A sample template can be found in the `data` folder.

```xml
<var id="transDepVar" type="lindep" dp="depVar/2" /> 
<var id="depVar" type="lindep" dp="uniformVar*0.3 + normalVar" />        
<var id="uniformVar" type="uniform" min="100" max="150" />        
<var id="normalVar" type="normal" mu="1.57" sd="0.12" />
```
	
