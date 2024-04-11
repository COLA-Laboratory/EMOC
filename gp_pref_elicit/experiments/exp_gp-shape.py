"""
@author: Luisa M Zintgraf (2017, Vrije Universiteit Brussel)

Experiments for figure 6 in the paper.
"""
import matplotlib.pyplot as plt
import numpy as np
import os
import sys
sys.path.insert(0, '.')
sys.path.insert(0, '..')
from gp_utilities import utils_experiment, utils_parameters
from mpl_toolkits import mplot3d

# plt.figure(figsize=(10, 7))
#
# num_obj = 2
# utl_noise = 0.01
#
# plt_idx = 1
#
# for seed in [10, 66]:
#
#     #for query_type in ['pairwise', 'ranking']:
#     for query_type in ['ranking']:
#
#         plt.subplot(2, 1, plt_idx)
#
#         params = utils_parameters.get_parameter_dict(query_type=query_type, num_objectives=num_obj,  utility_noise=utl_noise)
#         params['num queries'] = 10
#         params['seed'] = seed
#
#         # run the experiment
#         experiment = utils_experiment.Experiment(params)
#         result = experiment.run(recalculate=True)
#
#         # -- true utility --
#
#         # get the input domain
#         input_domain = result[1]
#         idx_order = np.argsort(-input_domain[:, 0])
#
#         # get the true utility function
#         true_utility = result[2]
#
#         # plot the true utility function
#         plt.plot(input_domain[:, 0][idx_order], true_utility[idx_order], 'k', linewidth=2, label='true utility')
#
#         # -- queried datapoints --
#
#         # get the datapoints
#         datapoints = result[5]
#         utility_datapoints = result[6]
#
#         # plot the datapoints
#         plt.plot(datapoints[:, 0], utility_datapoints, 'o', color='b', linewidth=5, label='datapoints')
#
#         # -- gaussian process --
#
#         # get mean and variance of gaussian process
#         gp_mean = result[3]
#         # normalise
#         gp_mean = (gp_mean-np.min(gp_mean)) / (np.max(gp_mean) - np.min(gp_mean))
#         gp_var = result[4]
#
#         # plot gp mean
#         plt.plot(input_domain[:, 0][idx_order], gp_mean[idx_order], '--', color='red', linewidth=3, label='GP mean')
#
#         # plot gp variance
#         plt.fill_between(input_domain[:, 0][idx_order], gp_mean[idx_order] - gp_var[idx_order],
#                          gp_mean[idx_order] + gp_var[idx_order],
#                          alpha=0.5, edgecolor='#CC4F1B', facecolor='#FF9848', label='GP variance')
#
#         # -- go to next plot --
#
#         if plt_idx == 1:
#             plt.title(query_type, fontsize=20)
#         if plt_idx == 2:
#             plt.title(query_type, fontsize=20)
#         if plt_idx == 2:
#             #plt.legend(fontsize=17, bbox_to_anchor=(-1.015, -0.25, 2, 1), loc=3, ncol=4, mode='expand')
#             #plt.legend(fontsize=17,bbox_to_anchor=(0,-0.3,2,10),loc=8,ncol=4,mode='expand')
#             plt.legend()
#         plt.yticks([])
#         plt.xticks([])
#         plt.xlim([np.min(input_domain[:, 0])-0.02, np.max(input_domain[:, 0])+0.02])
#         plt.ylim([np.min(gp_mean[idx_order] - gp_var[idx_order])-0.02, np.max(gp_mean[idx_order] + gp_var[idx_order])+0.02])
#         plt_idx += 1
#
# # -- show plot --
# plt.tight_layout(rect=(0, 0.08, 1, 1))
# dir_plots = './result_plots'
# if not os.path.exists(dir_plots):
#     os.mkdir(dir_plots)
# plt.savefig(os.path.join(dir_plots, 'gp_shape'))
# plt.show()

#simulate obj=3
obj_num=2
noise=1e-6

seed=13
query_type="ranking"

params=utils_parameters.get_parameter_dict(query_type=query_type,utility_noise=noise,num_objectives=obj_num)
params["num queries"]=20
params["seed"]=seed

experiment=utils_experiment.Experiment(params)
result=experiment.run(recalculate=True)

input_domain=result[1]
# print(input_domain)
order_index=np.argsort(-input_domain[:,0])
true_utility=result[2]
gp_mean=result[3]
print(gp_mean)
gp_mean=(gp_mean-np.min(gp_mean))/(np.max(gp_mean)-np.min(gp_mean))
gp_var=result[4]
datapoints=result[5]
utility_datapoints=result[6]

plt.plot(input_domain[:,1][order_index],true_utility[order_index],'k',linewidth=2,label='true_utility')
plt.plot(input_domain[:,1][order_index],gp_mean[order_index],'r',linewidth=2,label='gp')
plt.fill_between(input_domain[:,1][order_index],gp_mean[order_index]-gp_var[order_index],gp_mean[order_index]+gp_var[order_index],color='orange',alpha=0.5,label='variance')
plt.plot(datapoints[:,1],utility_datapoints,'o',label='samples')
plt.legend()
plt.xlabel('x2',fontsize=15)
plt.ylabel('y',fontsize=15)
plt.title('virtual utility function',fontsize=17)
plt.savefig('./result_plots/gp_utility.png')
plt.show()

# print(result.size)
print(np.array(gp_mean).shape)

# ax=plt.axes(projection='3d')
# plt.contourf(input_domain[:,0][order_index],input_domain[:,1],[order_index],true_utility[order_index])
# plt.show()
