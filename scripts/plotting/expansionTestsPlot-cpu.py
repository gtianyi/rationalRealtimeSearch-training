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
resultDirs = {"4x4-b500"}
limits = [3, 10, 30, 100, 300, 1000]
algorithms = ["A*", "F-Hat", "BFS", "Risk", "LSS-LRTA*"]

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

dfDiff['avg cpu time per action'] = dfDiff.groupby(['Node Expansion Limit','Algorithm'])['CPU Time Per Lookahead'].transform('mean')
dfDiff.round(2)

algorithmsExpA = ["A*", "F-Hat"]

algorithmsExpB = ["A*", "F-Hat", "BFS"]

algorithmsExpC = ["A*", "F-Hat", "BFS", "Risk", "LSS-LRTA*"]
#algorithmsExpC = ["A*", "F-Hat", "BFS", "Risk"]

print("building plots...")

for instance in resultDirs:
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    instanceDataExp = df.loc[df["instance"] == instance]
    instanceDataDiffExp = dfDiff.loc[dfDiff["instance"] == instance]

    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - A* Cost", instanceDataDiffExp, 0.35, "Algorithm",limits , algorithmsExpC, "Node Expansion Limit", "Algorithm Cost - A* Cost", "../../../plots/Experiment2CDifferenceNew" + instance + ".pdf")
    makeAvgLmPlot(11, 8, "avg cpu time per action", "Solution Cost", instanceDataDiffExp, "Node Expansion Limit", "Algorithm",None , algorithmsExpC, "avg cpu time per action", "Solution Cost", "../../../plots/Experiment2CLmAVG" + instance + ".pdf")
    makeLmPlot(11, 8, "CPU Time Per Lookahead", "Solution Cost", instanceDataDiffExp, "Node Expansion Limit", "Algorithm",None , algorithmsExpC, "cpu time per action", "Solution Cost", "../../../plots/Experiment2CLm" + instance + ".pdf")
