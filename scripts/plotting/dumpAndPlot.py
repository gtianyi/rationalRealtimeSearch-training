#!/usr/bin/env python

__author__ = 'tianyigu'

'''
This file includes some helper functions for collecting the training results

Author: Tianyi Gu
Date: 05/22/2019
'''

from collections import OrderedDict

#---------- Dump out h-hstar ---------------------------

def dump2file(h, hs, outFile):
    hsSet = set(hs)
    outFile.write(str(h)+' '+str(len(hs))+' ')
    for hsvalue in hsSet:
        valueCount = hs.count(hsvalue)
        outFile.write(str(hsvalue)+' '+str(valueCount)+' ')
    outFile.write("\n")

def dumphhstar(hhsCollection,dirName, roundHS = False):
    print("dumping out h-hstar collection...") 
    print("h count "+str(len(sampleCollection))) 

    f = open("../../../results/SlidingTilePuzzle/sampleData/"+dirName+"-statSummary.txt","w")
    od = OrderedDict(sorted(hhsCollection.items()))

    for h, hslist in od.items():
        if roundHS:
            #save to 0.1 to make histogramo
            hs = [round(v,2) for v in hs] # this is dangrous, but otherwise we have each data point a bin

        dump2file(h,hslist,f)

#---------- Dump out h-hat ---------------------------

def dumphhat(h, hs, outFile):
    outFile.write(str(h)+' '+str(sum(hs)/len(hs)))
    outFile.write("\n")

def dumphhat2file(hhatCollection,dirName):
    print("dumping out h-hat collection...") 
    print("h count "+str(len(sampleCollection))) 

    f_hhat = open("../../../results/SlidingTilePuzzle/sampleData/"+dirName+"-hhat.txt","w")
    od = OrderedDict(sorted(hhsCollection.items()))

    for h, hslist in od.items():
        dumphhat2file(h,hslist,f_hhat)

#---------- Dump out h and samples ---------------------------

def dump2file_sample_states(h, samples, outFile):
    outFile.write(str(h)+' '+str(len(samples))+' ')
    for sample in samples:
        outFile.write(sample+' ')
    outFile.write("\n")

def dumphSamples(sampleCollection, dirName):
    print("dumping out h-samples collection...") 
    print("h count "+str(len(sampleCollection))) 

    od = OrderedDict(sorted(sampleCollection.items()))

    f_samples = open("../../../results/SlidingTilePuzzle/sampleData/"+dirName+"-samples.txt","w")

    for h, samples in od.items():
        dump2file_sample_states(h,samples,f_samples)

#---------- plotting Hist---------------------------

def makeHistrogram(h, hs, fileDir, roundHS = False):
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.distplot(np.array(hs), kde=False)
    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    # ax.set_xscale('log')
    ylable = ''  
    if roundHS:
        ylable = '%.2f' % h  
    else:
        ylable = str(h)

    plt.ylabel("h="+ylable, color='black', fontsize=18)
    plt.xlabel("h*", color='black', fontsize=18)
    plt.savefig(fileDir+"h"+str(h).zfill(5)+".eps", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def plotHist(hhsCollection,dirName, roundHS = False):
    print("plot hist...") 
    print("h count "+str(len(hhsCollection))) 

    plotDir ="../../../plots/hist/"+dirName+"/"

    od = OrderedDict(sorted(hhsCollection.items()))

    for h, hslist in od.items():
        if len(hslist) > 0:
            makeHistrogram(h,hslist,plotDir, roundHS)

#---------- plotting Scatter---------------------------

def makeScatterPlot(h, hs,fileDir):
    df = pd.DataFrame({"h":h,"h*":hs})
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    # ax = sns.jointplot(x="h",y="h*",data=df, kind = "hex")
    ax = sns.scatterplot(x="h",y="h*",data=df, alpha = 0.01)
    plt.ylabel("h*", color='black', fontsize=18)
    plt.xlabel("h", color='black', fontsize=18)
    plt.savefig(fileDir+"scatter-inverse.pdf", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def plotScatter(all_h, all_hs, dirName):
    plotDir ="../../../plots/hist/"+dirName+"/"
    makeScatterPlot(all_h, all_hs, plotDir)


#---------- Dump point to csv ---------------------------

def dumpPoints2csv(h, hs,fileDir):
    df = pd.DataFrame({"h":h,"h*":hs})
    df.to_csv(fileDir+"h-h*.csv",index=None, header=True)
    return

def dump2csv(all_h, all_hs, dirName):
    csvDir ="../../../results/SlidingTilePuzzle/sampleData/" + dirName+"/"
    dumpPoints2csv(all_h, all_hs, csvDir)
