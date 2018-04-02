#!/bin/bash
# Mierne upraveny test skript z ukazkovych kodov

# Vstup ma mat prave 2 argumenty
# pocet_hodnot
# pocet_proc
if [ $# -ne 2 ];then 
    echo 'Vyzadovane 2 argumenty, num_count a proc_count'
    exit 1
fi;

num_count=$1;
proc_count=$2;

if [ $proc_count -gt $num_count ]; then
    echo 'Pocet procesorov musi byt rovny alebo mensi ako pocet cisel'
    exit 1
fi

# Preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o merge_split mss.cpp

# Vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$num_count of=numbers 2>/dev/null

# Spusteni
mpirun --oversubscribe --prefix /usr/local/share/OpenMPI \
       -np $proc_count merge_split $num_count

# Uklid
rm -f merge_split numbers
