#!/usr/bin/env python3
import csv
import string
import subprocess
from subprocess import run

nums = 20
proc = 1

with open('results.csv', 'w', newline='') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)
    spamwriter.writerow(['processors', 'tree-size', 'time'])
    string.ascii_lowercase
    for tree_size in range(2, 20):
        time_spent = 0.0
        experiment_count = 50
        for i in range(experiment_count):
            result = run('./test.sh {}'.format(string.ascii_lowercase[:tree_size]),
                         shell=True,
                         check=True,
                         stderr=subprocess.PIPE,
                         universal_newlines=True)
            time_spent += float(str(result.stderr).split("\n")[-2])
        spamwriter.writerow([proc, tree_size, time_spent/experiment_count])