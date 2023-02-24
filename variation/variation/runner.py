import numpy as np
import numexpr as ne
import argparse
import re
import xml.etree.ElementTree as ET
import copy
import os
import glob
import shutil
from variation import dependencySolver as ds
from ctypes import *


args = None #global args object

def is_var(arg): #method just checks if the string validates against the RE
    m = re.search('\$\{.*\}', arg)
    if m==None:
        return False
    else:    
        return True

def get_var_val(key, ii, varDict):
    """Gets an input in the likes of ${var} and returns the corresponding var value from the dict

    Parameters
    ----------
    key: string
        unparsed key of var
    ii: int
        current iteration idx
    varDict: dict
        variable dictionary

    Returns
    -------
    string
        variable value as string

    """
    res = varDict.get(key[2:-1], '0')[ii]
    return str(res)

def find_var(item, idx, varDict):
    """Recursively fills in the values of variables in the given data tree 

    Parameters
    ----------
    item: xml.etree.ElementTree.Element
        Tree item to traverse recursively
    idx: int
        current iteration idx
    vars: dict
        variable dictionary

    """
    for child in list(item):
        find_var(child, idx, varDict)
        for key, val in child.attrib.items():
            if is_var(val):
                child.attrib[key] = get_var_val(val, idx, varDict)

def hasValue(key, varDict):
    return key in varDict

def generateVar(var, n):    
    """Generates a value for a given var node

    Parameters
    ----------
    var: xml.etree.ElementTree.Element
        Tree node containing info about the variable
    n: int
        number of output files

    Returns
    -------
    array[n]
        n dimensional array containing the values (or linear equation) for the variable

    """
    
    dist = var.get('type')
    if dist == 'normal':
        val = np.random.normal(float(var.get('mu')), float(var.get('sd')), n)
    elif dist == 'uniform':            
        val = np.random.uniform(float(var.get('min')), float(var.get('max')), n)
    elif dist == 'lindep':
        val = np.full(n, str(var.get("dp")))       
    else:       
        raise ValueError("A wrong or invalid distribution type was provided")

    return val


def writeTreesToXML(n, tree, inpDir, nwName, varDict):
    """Saves n revs of the tree to the inpDir as an xml file

    Parameters
    ----------
    n:int
        number of revs that will be saved
    tree: ElementTree
        the tree struct generated from input
    inpDir: str
        input dir which contains the xml files
    nwName: str
        name of the output xml file
    varDict: dict
        dict containing array of vars
    """
  
    for i in range(n):
        cpTree = copy.deepcopy(tree)

        if not cpTree.getroot().find('vars') == None:
            cpTree.getroot().remove(cpTree.getroot().find('vars'))

        find_var(cpTree.getroot(), i, varDict)        
        tmpName = inpDir+ nwName + '_rev' + str(i) + '.xml'               
        cpTree.write(tmpName)


def executePipeline(tree, inpDir, varDict):
    """This method calls the roadGen Lib function for every xml file in the input dir

    Parameters
    ----------
    tree: ElementTree
        the tree struct generated from input
    inpDir: str
        input dir which contains the xml files
    varDict: dict
        dict containing array of vars
    
    """

    libpath = ""
    c = 0
        
    if os.name == "posix":  # if MacOS
        libpath = os.path.join(os.path.dirname(__file__), "resources/libroad-generation.so")          
        
    else:   
        libpath = os.path.join(os.path.dirname(__file__), "resources/road-generation.dll")           

    roadgen = cdll.LoadLibrary(libpath) #load shared lib

    xml_path = os.path.join(os.path.dirname(__file__), "resources/xml")#xml path argument for lib
    argXMLPath = c_char_p(xml_path.encode('utf-8'))

    for filename in os.listdir(inpDir):
        if filename.endswith(".xml"): 
            
            argName = (inpDir+filename)        
            argFilename = c_char_p(argName.encode('utf-8')) #execute "main" function from lib 
               
            roadgen.setSilentMode(c_bool(args.s))
            roadgen.setFileName(argFilename)
            roadgen.setXMLSchemaLocation(argXMLPath)
            if args.o:
                outArgs = c_char_p((inpDir + args.o+"_rev"+str(c)).encode('utf-8'))
                roadgen.setOutputName(outArgs)
            roadgen.execPipeline()
            c += 1
            

def initDirectories(inpDir):
    """This method inits the input directory

    Parameters
    ----------    
    inpDir: str
        input directory that will be created
    """
    if not os.path.exists(inpDir ):
        os.makedirs(inpDir )


def copyTemplate():
    """This method copies the example template to the current directory
    """
    shutil.copy(os.path.join(os.path.dirname(__file__), "resources/network.tmpl"), "network_example.tmpl")

def run():
    #parsing args-------------------------------------
    global args
    print("-- Let's start the road network variation")
    argParse = argparse.ArgumentParser()
    argParse.add_argument('-fname', help='path to template file', metavar='<TemplateFilename>')
    argParse.add_argument('-o', help='set output name', metavar='<out filename>')
    argParse.add_argument('-n', help='number of variations', metavar='<int>', type=int, default=20)
    argParse.add_argument('-e', help='generate example template', action='store_true')
    argParse.add_argument('-k', help='keep xml files', action='store_false')
    argParse.add_argument('-s', help='suppress console output', action='store_true')
    args = argParse.parse_args()    

    
    if args.e:
            copyTemplate()
            print("copied tmpl file!")

    if args.fname == None:
        if args.e == None:
            print("Error:\n-fname <FileName> required.\nUse -h for further help.")
        return

    n = args.n
    clearOutputFolder = args.k   
    fname = args.fname
    nwName = str.split(str.split(fname,'/')[-1],'.')[0]
    inpDir = os.path.join(os.path.dirname(fname), "variation_output/")
    initDirectories(inpDir)  

    #init --------------------------------------------
    tree = ET.parse(args.fname)
    vars = tree.getroot().find('vars')
    varDict = {}
    #reading values-----------------------------------
    if not vars == None:
        for var in vars:
            val = generateVar(var, n)
            varDict.update({var.get('id'): val})

    #solving equations------------------------------------

        varList = ds.getVarLists(varDict, n)
        varDict = ds.solveEQ(varList, n)
    else:
        print("No variables declared. Running only one iteration!")
        n = 1
    #clear input folder
    files = glob.glob(inpDir+'*')
    for f in files:
        os.remove(f)

    writeTreesToXML(n, tree, inpDir, nwName, varDict)
    executePipeline(tree, inpDir, varDict)

    #clear input folder (again)
    if clearOutputFolder :
        files = glob.glob(inpDir+'*.xml')
        for f in files:
            os.remove(f)

if __name__ == '__main__':
    run()