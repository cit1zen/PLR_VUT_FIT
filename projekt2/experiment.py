#!/usr/bin/env python3
import csv
import subprocess
from subprocess import run

nums = 20
proc = 1
# result = run('./test.sh {} {}'.format(nums, proc),
#            shell=True,
#            check=True,
#            stderr=subprocess.PIPE,
#            universal_newlines=True)
# # print(result)
# parsed = str(result.stderr).split("\n")[-4:-1]
# print(parsed)
# with open('results.csv', 'w', newline='') as csvfile:

#     spamwriter.writerow(['processors', 'numbers', 'time'])
#     spamwriter.writerow(parsed)

with open('results.csv', 'w', newline='') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)
    spamwriter.writerow(['processors', 'numbers', 'time'])
    for nums in [100, 500, 1000, 2500, 5000]:
        for proc in range(1, 11):
            time_spent = 0.0
            experiment_count = 50
            for i in range(experiment_count):
                result = run('./special-test.sh {} {}'.format(nums, proc),
                             shell=True,
                             check=True,
                             stderr=subprocess.PIPE,
                             universal_newlines=True)
                time_spent += float(str(result.stderr).split("\n")[-2])
            spamwriter.writerow([proc, nums, time_spent/experiment_count])