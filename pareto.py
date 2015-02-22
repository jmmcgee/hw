from phase1 import Simulation

import sys
import random
import matplotlib
import numpy as np
import matplotlib.pyplot as plt


# Simulation
def __main__(MAX_BUFFER, PARAM_MU, PARAM_LAMBDA):
    EVENTS_TO_SIMULATE =    10000
    MAX_BUFFER = int(MAX_BUFFER)
    PARAM_LAMBDA = float(PARAM_LAMBDA)
    PARAM_MU = float(PARAM_MU)

    def PARETO(minimum, shape):
        return minimum / ((1.0 - random.random()) ** (1.0 / shape)) 

    def ARRIVAL_INTERVAL():
        return PARETO(0.02, PARAM_MU)

    def PROCESSING_INTERVAL():
        return PARETO(0.02, PARAM_MU)

    simulation = Simulation(EVENTS_TO_SIMULATE, MAX_BUFFER, ARRIVAL_INTERVAL, PROCESSING_INTERVAL, quiet=True)
    simulation.run()
    (dropped, utilization, mean_queue_length) = simulation.get_statistics()

if __name__ == '__main__':
    __main__(*sys.argv[1:])
