#!/usr/bin/env python3

import re

def can_be_a_set_member_or_frozenset(item):
    '''Can the item be added to a set'''
    if isinstance(item, list) | isinstance(item, dict):
        return frozenset(item)
    else:
        return item

def all_subsets(lst):
    '''Generates a set of all subsets by given lst'''
    all_sets = []
    for i in range(1 << len(lst)):
        subset = [lst[bit] for bit in range(len(lst)) if i & (1 << bit) > 0]
        all_sets.append(subset)
    return all_sets

def all_subsets_excl_empty(*params, **exclude_empty):
    '''All subsets but not empty ones'''
    first_string = str(exclude_empty)
    second_string = (first_string[18::])
    if second_string == "False}":
        param=all_subsets(params)
    else:
        param=all_subsets(params)
        paraming=param.pop(0)

    return param
