#!/usr/bin/env python
# coding: utf-8

import time
from event import Event
from tqdm import trange, tqdm
import ROOT
import numpy as np
import matplotlib.pyplot as plt
import os
import tensorflow as tf
from dataclasses import dataclass
from typing import List
import pickle
import gzip

# tf.config.set_visible_devices([], 'GPU')
def save_dataset(dataset, filename, compression=False):
    print(f"Saving Dataset to {filename}... {'Using' if compression else 'Not using'} gzip compression.")
    if compression:
        f = gzip.open(filename, 'wb')
    else: f = open(filename, 'wb')
    pickle.dump(dataset, f)
    f.close()


print("Openning output.root")
file = ROOT.TFile.Open('output.root')
tree = file.tree
tree.Print()

## Must segregate files into chunks for memory constraints

max_events_per_file = 20

file_id = 0
dataset = []
entries = tree.GetEntriesFast()
print(f"Processing {entries} events...")
for event in tqdm(tree, total=entries):
    nu_energy = event.nu_energy
    e_energy = event.e_energy
    nu_direction = np.asarray(event.nu_direction)
    e_direction = np.asarray(event.e_direction)
    e_position = np.asarray(event.e_position)
    # write digits
    # annoyingly tf doesn't work on int16 sometimes...
    digit_map = np.array(event.digit_map.data(), dtype=np.int32)
    plane_map = np.reshape(digit_map, (12, 2560, 4492))
    plane_map = tf.sparse.from_dense(plane_map)
    e = Event(nu_energy,
              e_energy,
              nu_direction,
              e_direction,
              e_position,
              plane_map)
    dataset.append(e)
    if len(dataset) == max_events_per_file:
        filename = f"../data/ES_clean_{file_id}.pkl"
        save_dataset(dataset, filename, compression=False)
        dataset = []
        file_id += 1


