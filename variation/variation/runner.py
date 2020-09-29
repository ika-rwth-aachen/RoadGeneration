import numpy as np
import numexpr as ne
import argparse
import re
import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import copy
import os
import glob

dependendVars = []

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
    #print("generating var: ", var.get('id'))
    generatedValue = True
    dist = var.get('type')
    if dist == 'normal':
        val = np.random.normal(float(var.get('mu')), float(var.get('sd')), n)
    elif dist == 'uniform':            
        val = np.random.uniform(float(var.get('min')), float(var.get('max')), n)
    elif dist == 'lindep':
        generatedValue = False
        print("found linear dependency")
        val = np.zeros(n)
        dependendVars.append(var)
        
        #print(var.get("dp"))
        
    else:
        generatedValue = False
        raise ValueError("A wrong or invalid distribution type was provided")
    #print(type(val[0])) 
    return val, generatedValue

def resolveDependencies(varDict, n):

    while len(dependendVars) > 0:
        resolvedVal = False
        for var in dependendVars:
            print("resolving " + str(var.get("id")))
            li = np.zeros(n)
            for i in range(n) :                
                try:
                    tmp = float((eval(var.get("dp"), varDict.copy())))
                    li[i] = tmp
                    resolvedVal = True
                except:
                    resolvedVal = False          
            if resolvedVal :
                dependendVars.remove(var)
                varDict.update({var.get('id'): li})
                continue    
        if not resolvedVal :
            raise ValueError("Could not resolve dependencies!")
        

            
            
                
                

    val = li

def run():
    print("-- Let's start the road network variation")
    argParse = argparse.ArgumentParser()
    argParse.add_argument('-fname', required=True, help='filename of the road network template', metavar='TemplateFilename')
    argParse.add_argument('-n', help='number of variations to be generated', metavar='count', type=int, default=20)
    args = argParse.parse_args()
    
    n = args.n
    fname = args.fname
    nwName = str.split(str.split(fname,'/')[-1],'.')[0]
    inpDir = 'variation/data/inputs/'    
    tree = ET.parse(args.fname)
    vars = tree.getroot().find('vars')
    varDict = {}

    for var in vars:
        val, b = generateVar(var, n)
        if b:
            varDict.update({var.get('id'): val})
    resolveDependencies(varDict, n)

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

        print(varDict.get("halfAngleT")*0.3+varDict.get("radiusCR"))

        print(varDict)
        return# delete this ----------------------------------------------------------------------------------
        if os.name == "posix":  # if MacOS
            os.system(os.getcwd() + '/road-generation ' + tmpName)

        else:                   # if Windows
            os.system(os.getcwd() + '/roadGeneration.exe ' + tmpName)

    #clear input folder (again)
    #files = glob.glob(inpDir+'*.xml')
    #for f in files:
    #    os.remove(f)

    #count, bins, ignored = plt.hist(val, 30, density=True)
    #plt.plot(bins, 1/(sd * np.sqrt(2 * np.pi)) * np.exp( - (bins - mu)**2 / (2 * sd**2) ), linewidth=2, color='r')
    #plt.show()

if __name__ == '__main__':
    run()