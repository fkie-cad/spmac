import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np
import json
import random
import numpy as np
import itertools
from copy import deepcopy, copy
from multiprocessing import Pool
import pprint

### Code for an "intelligent" brute-forcing of optimal Golomb Rulers and g-Sidon Sets for different g and orders. Already computed rulers are stored in rulers.py

### Warning!!! Some computations for our paper took weeks on a server CPU

#optimal rulers as of wikipedia (is used to determine the optimal start point for some computations)
optimal_ruler = {
    1: [[0]],
    2: [[0,1]],
    3: [[0,1,3]],
    4: [[0, 1, 4, 6], [0, 2, 5, 6]],
    5: [[0,1,4,9,11], [0,2,7,8,11]],
    6: [[0,1,4,10,12,17],[0,1,4,10,15,17],[0,1,8,11,13,17],[0,1,8,12,14,17] ],
    7: [[0, 1, 4, 10, 18, 23, 25],[0, 1, 7, 11, 20, 23, 25],[0, 1, 11, 16, 19, 23, 25],[0, 2, 3, 10, 16, 21, 25],[0, 2, 7, 13, 21, 22, 25]],
    8: [[0, 1, 4, 9, 15, 22, 32, 34], [0, 2, 12, 19, 25, 30, 33, 34]],
    9: [[0, 1, 5, 12, 25, 27, 35, 41, 44]],
    10: [[0, 1, 6, 10, 23, 26, 34, 41, 53, 55]],
    11: [[0, 1, 4, 13, 28, 33, 47, 54, 64, 70, 72],[0, 1, 9, 19, 24, 31, 52, 56, 58, 69, 72]],
    12: [[0, 2, 6, 24, 29, 40, 43, 55, 68, 75, 76, 85]],
    13: [[0, 2, 5, 25, 37, 43, 59, 70, 85, 89, 98, 99, 106]],
    14: [[0, 4, 6, 20, 35, 52, 59, 77, 78, 86, 89, 99, 122, 127]],
    15: [[0, 4, 20, 30, 57, 59, 62, 76, 100, 111, 123, 136, 144, 145, 151]],
    16: [[0, 1, 4, 11, 26, 32, 56, 68, 76, 115, 117, 134, 150, 163, 168, 177]]
}

# how many sets of a given order and g (for generalized Sidon Sets) we want to compute
number_of_rulers = 64

# compute the number of times a certain distance between elements exists in the set
def ruler_diff(list, copies):
    if not is_sidon_set(list, copies):
        return None
    diffs = {}
    for i in range(len(list)):
        for j in range(i+1, len(list)):
            d = list[i] - list[j]
            if d<0:
                d=-d
            if d not in diffs:
                diffs[d]=0
            diffs[d]+=1
    return diffs

# check if this still fulfills the properties of a g-Sidon Set
def is_sidon_set(list, g):
    diffs = {}
    for i in range(len(list)):
        for j in range(i+1, len(list)):
            d = list[i] - list[j]
            if d<0:
                d=-d
            if d not in diffs:
                diffs[d] = 0
            diffs[d]+=1

            if diffs[d] > g:
                return False

    return True


def recursive_multisearch(curr, candidates, copies, target_order):

    res = []

    for cand in candidates:
        c = copy(curr)
        c.append(cand)

        diffs = ruler_diff(c, copies)
        if diffs == None:
            continue

        if len(c) == target_order:
            res.append( c )
            continue

        # check for every candiate number if it can be added to the set without violating the g-Sidon Set property
        new_cand = [number for number in candidates if number > cand]
        for d in diffs:
            if diffs[d] < copies:
                continue

            for n in c:
                temp = n-d
                if temp in new_cand:
                    new_cand.remove(temp)
                temp = n+d
                if temp in new_cand:
                    new_cand.remove(temp)

        if len(new_cand) >= target_order-len(c):
            r = recursive_multisearch( c, new_cand, copies, target_order )
            res += r

    return res

def multisearch_worker(input):
    curr = input[0]
    candidates = input[1]
    copies = input[2]
    target_order = input[3]
    res = recursive_multisearch(curr, candidates, copies, target_order)
    return res

def search_set(order, copies, count):

    # only if g= do we now the perfect starting point, i.e., the optimal GR
    if copies == 1:
        res = optimal_ruler[order]
        end = optimal_ruler[order][0][-1]+1
    else:
        res = []
        # we can adjust this for example if we now the that the short g-Sidon Set
        # of order X has the length Y, then we know that order X+1 has to be longer
        end = 2

    diffs = [0]

    while( len(res) < count ):

        pick_list = list(range(1,end))
        pick_index = 0
        jobs = []
        newly_found = []

        if order == 2:
            res.append( [0,end] )
            end += 1
            continue

        if order == 3:
            candidates = list(range(1,end))
            jobs.append( [ [0,end],candidates,copies,order] )
        else:
            for pick in range(1,end):
                input = [0,end,pick]
                candidates = list(range(pick+1,end))
                jobs.append( [input,candidates,copies,order] )

        with Pool(processes=4) as pool:
            results = pool.map( multisearch_worker, jobs )
            for r in results:
                newly_found += r

        end += 1
        print(f"Found {len(newly_found)} new sets of length {end-1}")

        if len(res) + len(newly_found) <= count:
            res += newly_found
        else:
            res += random.sample(newly_found, count - len(res))

    return res

def print_rulers(rulers):
    print("[",end="")
    for i, r in enumerate(rulers):
        print(sorted(r), end="")
        if i < len(rulers)-1:
            print(",")
        else:
            print("]")

if __name__ == "__main__":

    # The order of the request set
    length = [5,6,7,8,9]
    # This is g
    copies = [2]

    res = {}
    for c in copies:
        res[c] = {}
        for l in length:
            rul = search_set(l, c, number_of_rulers)
            print(f"Done with length: {l}, copies: {c}")
            print("\t\t\t{}: [".format(l), end="")
            for i, r in enumerate(rul):
                print(str(sorted(r)), end="")
                if i < len(rul)-1:
                    print(",\n\t\t\t",end="")
                else:
                    print("],")
