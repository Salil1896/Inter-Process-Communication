# Inter-Process-Communication-Merge-Sort-

The code first take No. of the node input in tree. After it takes
Adjacency matrix from the user. Using BFS traversal it calculates the Minimum 
Spanning Tree. This information is assigned globally. So it can be accessed by the
every child.After this it takes the data to be sorted from the user and pass the information to 
treeprocess. which creates child process recursively. In the treeprocess it creates the 
child process according to Minimum spanning tree. Each parent divides the data in the child 
equally and pass the data using PIPE. At the leaf node no further process is spawned and data 
are sorted. The code using Bubble sort to sort the data. After this child process again pass 
the data to its parent using pipe. Parent process Merge the sorted data as soon as it receives 
from its child and pass complete Merged data to its parent. Finally Root node receives all the 
data. The code finally print all the sorted data.
