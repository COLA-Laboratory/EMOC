import os

# problem = ["sphere", "ackley", "booth", "easom", "threehumpcamel", "styblinskitang", "alpine1", "hartmann", "sushi"]
# problem = ['booth']
problem = ['styblinskitang']
for iter in range(10):
    for pro in problem:
        os.system(f"python ./experiments/{pro}_runner.py {iter}")
        print(f"{pro}_runner.py finish")