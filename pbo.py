import os
import sys
import torch
from botorch.settings import debug
from torch import Tensor
sys.path.insert(0, './qEUBO')
from qEUBO.experiments.evalset.test_funcs import MOP
from qEUBO.src.experiment_manager import experiment_manager
import numpy

torch.set_default_dtype(torch.float64)
torch.autograd.set_detect_anomaly(False)
debug._set_state(False)

# def obj_func(X:Tensor, obj_num: int, ref: numpy.ndarray, sigma: float) -> Tensor:
#     mop = MOP(num_obj=obj_num, reference_point=ref, sigma=sigma)
#     objective_X = -torch.tensor(mop.do_evaluate(X)).squeeze(-1)
#     return objective_X

def pbo_mop(input_dim, input_X, algo, noise_type, noise_level_id, reference_point, sigma, first_trial, last_trial, query_num):
    input_X = numpy.array(input_X)

    def obj_func(X:Tensor, obj_num: int=input_dim, ref: numpy.ndarray=reference_point, sigma: float=sigma) -> Tensor:
        mop = MOP(num_obj=obj_num, input_X=input_X, reference_point=ref, sigma=sigma)
        objective_X = -torch.tensor(mop.do_evaluate(X)).squeeze(-1)
        return objective_X
    script_dir = os.path.dirname(os.path.realpath(sys.argv[0]))
    # print(script_dir[:-12])
    sys.path.append(script_dir[:-12])
    

    if noise_type == "logit":
        noise_levels = [0.53, 0.0128, 0.0255]

    noise_level = noise_levels[noise_level_id - 1]

    # Run experiment
    # if len(sys.argv) == 3:
    #     first_trial = int(sys.argv[1])
    #     last_trial = int(sys.argv[2])
    # elif len(sys.argv) == 2:
    #     first_trial = int(sys.argv[1])
    #     last_trial = int(sys.argv[1])
    # function = obj_func(X=[], obj_num=input_dim, ref=reference_point, sigma=sigma)

    max_post_mean_func = experiment_manager(
        problem="mop",
        obj_func=obj_func,
        input_dim=input_dim,
        noise_type=noise_type,
        noise_level=noise_level,
        algo=algo,
        num_alternatives=2,
        num_init_queries=2 * input_dim,
        num_algo_queries=query_num,
        first_trial=first_trial,
        last_trial=last_trial,
        restart=False,
    )
    best = (numpy.max(input_X, axis=0) - numpy.min(input_X, axis=0)) * numpy.array(max_post_mean_func) + numpy.min(input_X, axis=0)
    return best


if __name__ == "__main__":
    # Objective function
    input_dim = 2

    # input X
    # input_X = [[0.3,0.4],[0.2,0.5],[0.1,0.9],[0.2,0.8],[0.356,0.8673],[0.7632,0.6512],[0.623,0.3754]]
    # input_X = [[0,0]]
    input_X = [[0.3,0.4],[0.384,0.475],[0.3751,0.4123],[0.29784, 0.3891],[0.3023,0.4821],[0.3523, 0.390174]]

    # Algorithms
    algo = "qts"

    # Noise level
    noise_type = "logit"
    noise_level_id = 2
    
    # utility function
    reference_point = numpy.array([0.3,0.4])
    sigma = 0.1

    # num of trials
    first_trial = 0
    last_trial = 0

    # number of algorithm queries
    query_num = 20

    max_post_mean_func = pbo_mop(input_dim=input_dim, input_X=input_X, algo=algo, noise_type=noise_type, noise_level_id=noise_level_id, reference_point=reference_point, sigma=sigma, first_trial= first_trial, last_trial=last_trial, query_num=query_num)
    
    print(max_post_mean_func)