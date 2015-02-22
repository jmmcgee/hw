from phase1 import Simulation

import matplotlib

EVENTS_TO_SIMULATE =    10000
MAX_BUFFER = int(MAX_BUFFER)
PARAM_LAMBDA = float(PARAM_LAMBDA)
PARAM_MU = float(PARAM_MU)

def NEG_EXP(param):
    # random.random is uniform over [0.0, 1.0)
    # math.log is base-e with no base kwarg
    return (-1.0 / param) * math.log(1.0 - random.random())

def PARETO(minimum, shape):
    return minimum / ((1.0 - random.random()) ** (1.0 / shape)) 

def ARRIVAL_INTERVAL():
    return NEG_EXP(PARAM_LAMBDA)

def PROCESSING_INTERVAL():
    return NEG_EXP(PARAM_MU)

def ARRIVAL_PARETO():
    return PARETO(0.02, PARAM_MU)