import sys
import numpy as np
import functools
import operator
sys.path.append("./")
import EMOC

class MyProblem(EMOC.Problem):
    def __init__(self, dec_num, obj_num):
        super(MyProblem, self).__init__(dec_num,obj_num)
        lower_bound = [0] * dec_num
        self.lower_bound = lower_bound
        self.upper_bound = [1] * dec_num
        self.count = 0

    def CalObj(self, ind):
        self.count = self.count + 1
        x = ind.dec
        temp_obj = [0] * self.obj_num
        # print("here\n")
        k = self.dec_num-self.obj_num+1
        sum1 = 0
        for i in range(k):
            sum1+= (x[self.obj_num-1+i]-0.5)**2 - np.cos(20*np.pi*(x[self.obj_num-1+i]-0.5))
        g = 100*(k+sum1)

        for i in range(self.obj_num):
            temp_obj[i] = 0.5*(1.0+g)

        for i in range(self.obj_num):
            temp_obj[i] *= functools.reduce(operator.mul,
                                       [x for x in x[:self.obj_num-i-1]],1
                                       )
            if i>0:
                temp_obj[i]*= 1- x[self.obj_num-i-1]       # dec = ind.dec_test
        ind.obj = temp_obj


class MyUF1(EMOC.Problem):
    def __init__(self, dec_num, obj_num):
        super(MyUF1, self).__init__(dec_num,obj_num)
        lower_bound = [-1] * dec_num
        lower_bound[0] = 0
        self.lower_bound = lower_bound
        self.upper_bound = [1] * dec_num

    def CalObj(self, ind):
        x = ind.dec
        temp_obj = [0] * self.obj_num
        # print("here\n")
        sum1 = 0
        count1 = 0
        sum2 = 0
        count2 = 0
        for i in range(2,self.dec_num+1):
            yj = x[i-1] - np.sin(6.0 * np.pi * x[0] + i * np.pi / self.dec_num)
            yj = yj * yj
            if i % 2 == 0:
                sum2 += yj
                count2 = count2 + 1
            else:
                sum1 += yj
                count1 += 1
        temp_obj[0] = x[0] + 2.0 * sum1 / count1
        temp_obj[1] = 1.0 - np.sqrt(x[0]) + 2.0 * sum2 / count2
        ind.obj = temp_obj

# create and set EMOC parameters
para = EMOC.EMOCParameters()
para.algorithm_name = "NSGA2"
para.problem_name = "ZDT1"
para.population_num = 100
para.decision_num = 7
para.objective_num = 2
para.max_evaluation = 25000
para.output_interval = 10000

# set customized problem (optional)
# myProblem = MyProblem(para.decision_num,para.objective_num)
# myProblem = MyUF1(para.decision_num,para.objective_num)
# para.SetProblem(myProblem)

# set initial problem (optional)
# initial_pop = np.random.random((100,7))
# para.SetInitialPop(initial_pop)

# create EMOCManager instance and run it
EMOCManager = EMOC.EMOCManager()
EMOCManager.SetTaskParameters(para)
EMOCManager.Run()

result = EMOCManager.GetResult()
print("IGD: ",result.igd)
print("HV: ",result.hv)
print("Runtime: ",result.runtime)
print("Population Number: ",result.pop_num)
print("Population Decisions: ",result.pop_decs)
print("Population Objectives: ",result.pop_objs)
