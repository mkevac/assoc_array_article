#!/usr/bin/env python

from __future__ import print_function

import subprocess
import json
import sys

assocarraytypes = ["judy", "std", "spp", "dense", "sparse"]
hashtypes = ["std", "xxhash", "t1ha"]
memoryallocators = ["std", "jemalloc"]
keys = 10000000
bestof = 3


def getRandomFindsTime(output):
    for line in output:
        elems = line.split(",")
        if elems[3] == "random finds":
            return float(elems[4])
    raise Exception("random finds not found")


def run_test(aa, ht, ma, count):
    binary = "./hash_judy_test" if ma == "std" else "./hash_judy_test_jemalloc"
    cmd = ["/usr/bin/time", "-f", "%M",
           binary, aa, ht, str(count)]

    bestAttemptRandomFindsTime = None
    bestAttemptResults = None

    for attempt in range(bestof):
        sys.stderr.write("Running attempt {} of {} for {} {} {} {}\n".format(
              attempt+1, bestof, aa, ht, ma, count))

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE)
        stdoutdata, stderrdata = proc.communicate()

        stdoutlines = stdoutdata.split("\n")
        stderrlines = stderrdata.split("\n")

        randomFindsTime = getRandomFindsTime(stdoutlines)

        if bestAttemptRandomFindsTime:
            if bestAttemptRandomFindsTime > randomFindsTime:
                bestAttemptRandomFindsTime = randomFindsTime
                bestAttemptResults = [stdoutlines, stderrlines]
        else:
            bestAttemptRandomFindsTime = randomFindsTime
            bestAttemptResults = [stdoutlines, stderrlines]

    results = {}
    results["memory"] = int(bestAttemptResults[1][0])
    results["tests"] = []

    for line in bestAttemptResults[0]:
        if line == "":
            break

        _, _, _, test, time = line.split(",")

        tests = results["tests"]
        tests.append([test, float(time)])
        results["tests"] = tests

    return results


def main():
    results = []

    for ma in memoryallocators:
        for aa in assocarraytypes:
            htt = hashtypes
            if aa == "judy":
                htt = ["std"]
            for ht in htt:
                r = run_test(aa, ht, ma, keys)
                r["aa"] = aa
                r["ht"] = ht
                r["ma"] = ma
                results.append(r)

    print(json.dumps(results))

if __name__ == "__main__":
    main()
