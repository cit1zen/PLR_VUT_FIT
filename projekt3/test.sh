#!/bin/bash
# Mierne upraveny test skript z ukazkovych kodov

if [ $# -ne 1 ];then 
    echo 'Ocakavany je len 1 argument, postupnost'
    exit 1
fi;

binary_tree=$1;

proc_count=$(expr ${#binary_tree} \* 2 - 2)

# Preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o preorder pr.cpp

# Spusteni
mpirun --oversubscribe --prefix /usr/local/share/OpenMPI \
       -np $proc_count preorder $binary_tree

# Uklid
rm -f preorder numbers
