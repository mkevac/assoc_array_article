#!/usr/bin/python

import sys
import json


def format_for_speed(results):
    formatted = []

    for result in results:
        if result["ma"] != "std" or result["ht"] != "std":
            continue
        tests = result["tests"]
        for test in tests:
            test_name, test_time = test
            if test_name != "random finds":
                continue
            formatted.append([result["aa"], test_time])

    return sorted(formatted)

def format_for_memory(results):
    formatted = []

    for result in results:
        if result["ma"] != "std" or result["ht"] != "std":
            continue
        formatted.append([result["aa"], result["memory"]])

    return sorted(formatted)

def format_for_hash_functions(results):
    formatted = []

    for result in results:
        if result["ma"] != "std":
            continue
        tests = result["tests"]
        for test in tests:
            test_name, test_time = test
            if test_name != "random finds":
                continue
            formatted.append(["{} {}".format(result["aa"], result["ht"]), test_time])

    return sorted(formatted)

def format_for_jemalloc(results):
    formatted = []

    for result in results:
        if result["ht"] != "std":
            continue
        tests = result["tests"]
        for test in tests:
            test_name, test_time = test
            if test_name != "random finds":
                continue
            formatted.append(["{} {}".format(result["aa"], result["ma"]), test_time])

    return sorted(formatted)

def format_for_jemalloc_memory(results):
    formatted = []

    for result in results:
        if result["ht"] != "std":
            continue
        formatted.append(["{} {}".format(result["aa"], result["ma"]), result["memory"]])

    return sorted(formatted)


results = None

with open(sys.argv[1]) as fp:
    results = json.load(fp)

formatted = {
    "speed": format_for_speed(results),
    "memory": format_for_memory(results),
    "hash_functions": format_for_hash_functions(results),
    "jemalloc": format_for_jemalloc(results),
    "jemalloc_memory": format_for_jemalloc_memory(results)
}

html_template = open('charts-template.html', 'r').read()
resulting_html = html_template.replace('__DATA__', "chart_data = " + json.dumps(formatted))
open('charts.html', 'w').write(resulting_html)
