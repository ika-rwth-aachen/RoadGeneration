import numpy as np
import numexpr as ne
import argparse
import sympy as sym
import re
import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import copy
import os
import glob
import dependencySolver as ds



def is_var(arg):
    m = re.search('\$\{.*\}', arg)
    if m==None:
        return False
    else:    
        return True

def get_var_val(key, ii, varDict):
    res = varDict.get(key[2:-1], '0')[ii]
    return str(res)

def find_var(item, idx, vars):
    for child in item.getchildren():
        find_var(child, idx, vars)
        for key, val in child.attrib.items():
            if is_var(val):
                child.attrib[key] = get_var_val(val, idx, vars)

def hasValue(key, varDict):
    return key in varDict

def generateVar(var, n):    
    
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
    """runs the program"""


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
        
        if os.name == "posix":  # if MacOS
            os.system(os.getcwd() + '/../road-generation ' + tmpName)

        else:                   # if Windows
            os.system(os.getcwd() + '/../roadGeneration.exe ' + tmpName)

    #clear input folder (again)
    if clearOutputFolder :
        files = glob.glob(inpDir+'*.xml')
        for f in files:
            os.remove(f)

if __name__ == '__main__':
    run()