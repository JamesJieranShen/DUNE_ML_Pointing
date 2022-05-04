from dataclasses import dataclass
import numpy as np
import tensorflow as tf

@dataclass
class Event:
    nu_energy: float
    e_energy : float
    nu_direction: np.ndarray
    e_direction : np.ndarray
    e_position  : np.ndarray
    digit_map   : tf.sparse