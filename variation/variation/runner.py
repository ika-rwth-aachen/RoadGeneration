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


class roadConfig(Structure):
    """Config struct which is passed to the road-generation library
    """
    _fields_ = [("verbose", c_bool),
                ("filename", c_char_p),
                ("outname", c_char_p),
                ("xmllocation", c_char_p)]


class config:
    fname: str
    outputFilePath: str
    o: str
    n: int = 20
    v: bool = True
    e: bool = True
    k: bool = True
    writtenFiles: list = []
    inputDir: str
    filenameRaw: str

def parse_args():
    #parsing args-------------------------------------
    cfg = config()

    argParse = argparse.ArgumentParser()
    argParse.add_argument('-fname', help='path to template file', metavar='<TemplateFilename>')
    argParse.add_argument('-o', help='set output name', metavar='<out filename>')
    argParse.add_argument('-n', help='number of variations', metavar='<int>', type=int, default=20)
    argParse.add_argument('-e', help='generate example template', action='store_true')
    argParse.add_argument('-k', help='keep xml files', action='store_true')
    argParse.add_argument('-v', help='print console output', action='store_false')
    args = argParse.parse_args()    

    
    if args.e:
            copyTemplate()
            print("copied tmpl file!")
            exit(0)

    if args.fname == None:
        if args.e == None:
            print("Error:\n-fname <FileName> required.\nUse -h for help.")
        return

    cfg.fname = args.fname
    cfg.v = args.v
    cfg.e = args.e
    cfg.n = args.n
    cfg.k = args.k
    cfg.o = args.o
    cfg.inputDir = os.path.dirname(cfg.fname)
    cfg.filenameRaw = str.split(str.split(cfg.fname,'/')[-1],'.')[0]


    return cfg

def is_var(arg): #method checks if the string validates against the RE
    m = re.search('\$\{.*\}', arg)
    if m==None:
        return False
    else:    
        return True

def get_var_val(key, ii, varDict):
    """Gets an input in form of ${var} and returns the corresponding var value from the dict

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


def writeTreesToXML(cfg, tree, varDict):
    """Saves n revs of the tree to the inpDir as an xml file

    Parameters
    ----------
    n:int
        number of revs that will be saved
    tree: ElementTree
        the tree struct generated from input
    varDict: dict
        dict containing array of vars
    """
  
    for i in range(cfg.n):
        cpTree = copy.deepcopy(tree)

        if not cpTree.getroot().find('vars') == None:
            cpTree.getroot().remove(cpTree.getroot().find('vars'))

        find_var(cpTree.getroot(), i, varDict)        
        tmpName = f"{cfg.outputFilePath}_rev{i}.xml"
        cpTree.write(tmpName)
        cfg.writtenFiles.append(tmpName)


def executePipeline(cfg):
    """This method calls the roadGen Lib function for every xml file in the input dir

    Parameters
    ----------
    cfg: config
        Config struct
    """

    libpath = ""
        
    if os.name == "posix":  # if MacOS
        libpath = os.path.join(os.path.dirname(__file__), "resources/libroad-generation.so")          
        
    else:   
        libpath = os.path.join(os.path.dirname(__file__), "resources/road-generation.dll")           

    roadgen = cdll.LoadLibrary(libpath) #load shared lib

    xml_path = os.path.join(os.path.dirname(__file__), "resources/xml")#xml path argument for lib
    argXMLPath = c_char_p(xml_path.encode('utf-8'))


    for filename in os.listdir(os.path.dirname(cfg.outputFilePath)):
        if filename.endswith(".xml"): 
            filenamearg = os.path.join(os.path.dirname(cfg.outputFilePath), filename)

            rcfg = roadConfig()
            rcfg.verbose = cfg.v
            rcfg.filename =  c_char_p(filenamearg.encode('utf-8'))
            rcfg.outname = c_char_p(filenamearg[:-4].encode('utf-8'))
            rcfg.xmllocation = argXMLPath
            roadgen.executePipelineCfg(rcfg)



def copyTemplate():
    """This method copies the example template to the current directory
    """
    shutil.copy(os.path.join(os.path.dirname(__file__), "resources/network.tmpl"), "network_example.tmpl")


def run_variation(fname, n=20, v=True, k=False, o=None, e=False):
    cfg = config()
    cfg.v = v
    cfg.e = e
    cfg.n = n
    cfg.k = k
    cfg.o = o


    cfg.inputDir =  os.path.dirname(fname)
    cfg.filenameRaw =  str.split(str.split(fname,'/')[-1],'.')[0]
    cfg.fname = fname
    if cfg.e:
        copyTemplate()
        print("copied .tmpl file!")

    execute(cfg)

def execute(cfg):

    #init --------------------------------------------
    tree = ET.parse(cfg.fname)
    vars = tree.getroot().find('vars')
    varDict = {}

    #handle file and output names
    if cfg.o == None:
        cfg.outputFilePath =  os.path.join(os.path.join(cfg.inputDir, "variation_output/"), cfg.filenameRaw)       

    else: 
        if cfg.o[-1] == "/":
            cfg.o = os.path.join(cfg.o, cfg.filenameRaw)

        cfg.outputFilePath = cfg.o


    if not os.path.exists(os.path.dirname(cfg.outputFilePath)) and os.path.dirname(cfg.outputFilePath) != "":
        os.makedirs(os.path.dirname(cfg.outputFilePath))
        print(f"directory created: {os.path.dirname(cfg.outputFilePath)}")

    #reading values-----------------------------------
    if not vars == None:
        for var in vars:
            val = generateVar(var, cfg.n)
            varDict.update({var.get('id'): val})

    #solving equations------------------------------------

        varList = ds.getVarLists(varDict, cfg.n)
        varDict = ds.solveEQ(varList, cfg.n)
    else:
        print("No variables declared. Running only one iteration!")
        n = 1
    #clear input folder only if its default folder
    if cfg.o == None:
        files = glob.glob(os.path.dirname(cfg.outputFilePath)+ "/*.xodr")
        for f in files:
            os.remove(f)

    writeTreesToXML(cfg, tree, varDict)
    executePipeline(cfg)

    #clear input folder (again)
    if not cfg.k:
        for f in cfg.writtenFiles:
            os.remove(f)

def run():
    print("-- Let's start the road network variation")
    cfg = parse_args()
    execute(cfg)
    print("done!")

if __name__ == '__main__':
    run()