import sympy as sy

ab = 5
b = 4
dep = "a + 0.5 * x"
transDep = "3+dep"

var = ("c", dep)
unresDep = []
unresDep.append(var)

varDict = {}

varDict.update({"a": ab})
varDict.update({"b": b})

print(varDict)
print(unresDep)
x = sy.Symbol('x')
a = sy.Symbol('a')
b = sy.Symbol('b')
lindep = sy.Symbol('lindep')

sol = sy.solve(["3-x" , "a-x+1", dep+"-"+"lindep"],  (x,a, lindep))
print(len(sol))
print(sol[a])
print(sol[x])
print(sol[lindep])

