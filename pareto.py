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
    return simulation.get_statistics()

    
#if __name__ == '__main__':
#    __main__(*sys.argv[1:])

class Plot:
    x_vals = []
    y_vals = []

departure_rate = 1
plt.figure(1)

utilized_plot = Plot()
utilized_plot.x_vals = []
utilized_plot.y_vals = []
for arrival_rate in (0.1,0.25,0.4,0.55,0.65,0.8,0.9):
    (dropped, utilization, mean_queue_length) = __main__(0, arrival_rate, departure_rate)
    utilized_plot.x_vals.append(arrival_rate)
    utilized_plot.y_vals.append(utilization)
print utilized_plot.x_vals
print utilized_plot.y_vals
plt.subplot(211)
plt.plot(utilized_plot.x_vals, utilized_plot.y_vals, 'ro')
plt.title('Utilization of M/M/1 Queue with Infinite buffer')
plt.ylabel('utilization')

mean_queue_length_plot = Plot()
mean_queue_length_plot.x_vals = []
mean_queue_length_plot.y_vals = []
for arrival_rate in (0.1,0.25,0.4,0.55,0.65,0.8,0.9):
    (dropped, utilization, mean_queue_length) = __main__(0, arrival_rate, departure_rate)
    mean_queue_length_plot.x_vals.append(arrival_rate)
    mean_queue_length_plot.y_vals.append(mean_queue_length)
print mean_queue_length_plot.x_vals
print mean_queue_length_plot.y_vals
plt.subplot(212)
plt.plot(mean_queue_length_plot.x_vals, mean_queue_length_plot.y_vals, 'ro')
plt.title('Mean Queue length of M/M/1 Queue with Infinite buffer')
plt.xlabel('arrival_rate')
plt.ylabel('mean_queue_length')
plt.show()
