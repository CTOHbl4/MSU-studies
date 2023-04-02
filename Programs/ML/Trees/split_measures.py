import numpy as np


def evaluate_measures(sample):
    smpl = np.array(sample)
    size = smpl.size
    un, count = np.unique(smpl, return_counts=True)
    fr = count/size
    entropy = fr * np.log(fr)
    gini = 1 - np.sum(fr**2)
    entropy = -np.sum(entropy)
    error = 1 - max(fr)
    measures = {'gini': gini, 'entropy': entropy, 'error': error}
    return measures
