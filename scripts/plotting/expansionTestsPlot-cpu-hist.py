import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import json
import seaborn as sns
import numpy as np
from os import listdir

def makeDifferencePlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, hue_order=hueOrderList, data=dataframe, ci=95, join=False, dodge=dodge, palette="Set2")
    ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def makeAvgLmPlot(width, height, xAxis, yAxis, dataframe, style, hue, orderList, hueOrderList, xLabel, yLabel, outputName):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.scatterplot(x=xAxis, y=yAxis, hue=hue, style=style, hue_order=hueOrderList, data=dataframe, ci=95, palette="Set2")
    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    ax.set_xscale('log')
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def makeLmPlot(width, height, xAxis, yAxis, dataframe, style, hue, orderList, hueOrderList, xLabel, yLabel, outputName):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.scatterplot(x=xAxis, y=yAxis, hue=hue, style=style, hue_order=hueOrderList, data=dataframe, ci=95, palette="Set2")
    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    ax.set_xscale('log')
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

# Hard coded result directories
#resultDirs = {"4x4-b5","4x4-b10","4x4-b50","4x4-b100","4x4-b500"}
resultDirs = {"4x4_old-b5","4x4_old-b10","4x4_old-b50","4x4_old-b100"}
limits = [3, 10, 30, 100, 300, 1000]
algorithms = ["Risk"]

instance = []
lookAheadVals = []
algorithm = []
solutionCost = []
differenceCost = []
cpuTimePerLookahead = []
cpuTimePerLookaheadLog = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../../../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir):
        with open("../../../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instance.append(str(dir))
                lookAheadVals.append(resultData["Lookahead"])
                algorithm.append(algo)
                solutionCost.append(resultData[algo])
                differenceCost.append(resultData[algo] - resultData["A*"])
                cpuTimePerLookahead.append(resultData[algo+"-CPUTime"])

df = pd.DataFrame({
    "instance":instance,
    "Node Expansion Limit":lookAheadVals,
    "Solution Cost":solutionCost,
    "Algorithm":algorithm
})

dfDiff = pd.DataFrame({
    "instance":instance,
    "Node Expansion Limit":lookAheadVals,
    "Solution Cost":solutionCost,
    "Algorithm Cost - A* Cost":differenceCost,
    "Algorithm":algorithm,
    "CPU Time Per Lookahead":cpuTimePerLookahead
})

dfDiff['avg cpu time per action'] = dfDiff.groupby(['Node Expansion Limit','instance'])['CPU Time Per Lookahead'].transform('mean')
dfDiff.round(2)

algorithmsExpA = ["A*", "F-Hat"]

algorithmsExpB = ["A*", "F-Hat", "BFS"]

algorithmsExpC = ["A*", "F-Hat", "BFS", "Risk", "LSS-LRTA*"]
#algorithmsExpC = ["A*", "F-Hat", "BFS", "Risk"]

#instanceExp = ["4x4-b5","4x4-b10","4x4-b50","4x4-b100","4x4-b500"]
instanceExp = ["4x4_old-b5","4x4_old-b10","4x4_old-b50","4x4_old-b100"]

print("building plots...")

for alg in algorithms:
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    instanceDataExp = df.loc[df["Algorithm"] == alg]
    instanceDataDiffExp = dfDiff.loc[dfDiff["Algorithm"] == alg]

    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - A* Cost", instanceDataDiffExp, 0.35, "instance",None , instanceExp, "Node Expansion Limit", "Algorithm Cost - A* Cost", "../../../plots/Experiment2CDifference" + alg + ".pdf")
    makeAvgLmPlot(11, 8, "avg cpu time per action", "Solution Cost", instanceDataDiffExp, "Node Expansion Limit", "instance",None , instanceExp, "avg cpu time per action", "Solution Cost", "../../../plots/InstanceExpLmAVG" + alg + ".pdf")
    makeLmPlot(11, 8, "CPU Time Per Lookahead", "Solution Cost", instanceDataDiffExp, "Node Expansion Limit", "instance",None , instanceExp, "cpu time per action", "Solution Cost", "../../../plots/InstanceExpLm" + alg + ".pdf")
