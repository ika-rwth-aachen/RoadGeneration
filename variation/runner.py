import numpy as np
import argparse
import re
import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import copy
import os
import glob

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
        print("generating var: ", var.get('id'))
        dist = var.get('type')
        if dist == 'normal':
            val = np.random.normal(float(var.get('mu')), float(var.get('sd')), n)
        elif dist == 'uniform':            
            val = np.random.uniform(float(var.get('min')), float(var.get('max')), n)
        else:
            raise ValueError("A wrong or invalid distribution type was provided")
        varDict.update({var.get('id'): val})
        

    #print(varDict)
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
            os.system(os.getcwd() + '/road-generation ' + tmpName)

        else:                   # if Windows
            os.system(os.getcwd() + '/roadGeneration.exe ' + tmpName)

    #clear input folder (again)
    files = glob.glob(inpDir+'*.xml')
    for f in files:
        os.remove(f)

    #count, bins, ignored = plt.hist(val, 30, density=True)
    #plt.plot(bins, 1/(sd * np.sqrt(2 * np.pi)) * np.exp( - (bins - mu)**2 / (2 * sd**2) ), linewidth=2, color='r')
    #plt.show()

if __name__ == '__main__':
    run()