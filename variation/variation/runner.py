import numpy as np
import numexpr as ne
import argparse
import re
import xml.etree.ElementTree as ET
import copy
import os
import glob
import dependencySolver as ds
from ctypes import *


def is_var(arg): #this method just checks if the string validates against the RE
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

def find_var(item, idx, vars):
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
    for child in item.getchildren():
        find_var(child, idx, vars)
        for key, val in child.attrib.items():
            if is_var(val):
                child.attrib[key] = get_var_val(val, idx, vars)

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


def run():
    #parsing args-------------------------------------
    print("-- Let's start the road network variation")
    argParse = argparse.ArgumentParser()
    argParse.add_argument('-fname', required=True, help='filename of the road network template', metavar='TemplateFilename')
    argParse.add_argument('-n', help='number of variations to be generated', metavar='count', type=int, default=20)
    argParse.add_argument('-k', help='keep intermediate xml files after generation', action='store_false')

    args = argParse.parse_args()    
    n = args.n
    clearOutputFolder = args.k   
    fname = args.fname
    nwName = str.split(str.split(fname,'/')[-1],'.')[0]
    inpDir = 'data/inputs/'    

    #init --------------------------------------------
    tree = ET.parse(args.fname)
    vars = tree.getroot().find('vars')
    varDict = {}
    #reading values-----------------------------------

    for var in vars:
        val = generateVar(var, n)
        varDict.update({var.get('id'): val})

    #solving equations------------------------------------

    varList = ds.getVarLists(varDict, n)
    varDict = ds.solveEQ(varList, n)

    #clear input folder
    files = glob.glob(inpDir+'*')
    for f in files:
        os.remove(f)

    for i in range(n):
        cpTree = copy.deepcopy(tree)
        cpTree.getroot().remove(cpTree.getroot().find('vars'))        
        
        find_var(cpTree.getroot(), i, varDict)        
        tmpName = inpDir+ nwName + '_rev' + str(i) + '.xml'
        cpTree.write(tmpName)
        
        libpath = os.path.abspath("../build/libroadgen.so")  
        if os.name == "posix":  # if MacOS
            roadgen = cdll.LoadLibrary(libpath) #load shared lib
            arg = c_char_p(tmpName.encode('utf-8')) #execute "main" function from lib
            roadgen.callMain(arg)

        else:                   # TODO: handle windows os
            os.system(os.getcwd() + '/../roadGeneration.exe ' + tmpName)

    #clear input folder (again)
    if clearOutputFolder :
        files = glob.glob(inpDir+'*.xml')
        for f in files:
            os.remove(f)

if __name__ == '__main__':
    run()