#!/usr/bin/python

import sys
import xml.etree.ElementTree as ET

threshold = 0.0001


def tree_to_list(tree):
    x = []

    return parse_tree_to_list(tree, x)


def parse_tree_to_list(tree, x):
    x.append(tree.tag)
    elem = tree.attrib
    for a,b in elem.items() :
        x.append(a)
        x.append(b)

  
    for child in tree:
        parse_tree_to_list(child,x)
    return x

def print_childs_r(tree, lv):
    intend = ""
    for i in range(lv):
        intend += "  "
        
    print (intend, tree.tag, tree.attrib)
    for child in tree :     
      
        print_childs_r(child, (lv+1))
    print (intend, " /" + tree.tag)


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


def cmp_xml_list(list1, list2):
    s_diff = 0
    n_diff = 0
    e_diff = 0
    if len(list1) != len(list2):
        print("missmatch ", len(list1), len(list2))
        return 1 
    
  
    for i in range (len(list1)):
        if(list1[i] != list2[i]) : 
            if not is_number(list1[i] or not is_number(list2[i])): 
                s_diff += 1
                continue

            number1 = float(list1[i])
            number2 = float(list2[i])

            if abs(number1-number2 > threshold): 
                e_diff += 1
            else:
                n_diff += 1
                print ("diff: ", list1[i], list2[i])

   
    return (s_diff, n_diff, e_diff)



args = sys.argv

file1 = args[1]
file2 = args[2]

tree1 = ET.parse(file1)
root1 = tree1.getroot()

tree2 = ET.parse(file2)
root2 = tree2.getroot()

list1 = tree_to_list(root1)
list2 = tree_to_list(root2)

s,n,e = cmp_xml_list(list1, list2)

print ("\nsmall numerical diffs :", n)
print ("numerical diffs       :", e)
print ("string diffs          :", s)

sys.exit(s+e)


