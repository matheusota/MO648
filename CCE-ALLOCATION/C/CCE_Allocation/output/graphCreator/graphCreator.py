import sys
import pandas as pd
import numpy as np
from matplotlib import rcParams
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import argparse
from math import sqrt

# parse arguments from stdin
parser = argparse.ArgumentParser()
parser.add_argument("filename", help="input data filename", type = str)
parser.add_argument("yname", help="y-axis name", type = str)
args = parser.parse_args()

filename = args.filename
yname = args.yname

# read from file to a pandas dataframe
df = pd.DataFrame(columns=["algorithm", "number of users", yname])

file = open("../" + filename + ".csv")

i = 0
for line in file:
    line = line.strip()
    line = line.split(";")
    df.loc[i] = [line[0], int(line[1]), float(line[2]) / 1000.0]
    i += 1

# set some plot parameters
#dashes = ['-', ':', '-.', '--','-', ':', '-.', '--']
dashes = ['-']
markers = ['o', 's', '^', 'p', 'h']
colors = ['blue', 'red', 'gray', 'green', 'brown', 'yellow']

pgf_with_custom_preamble = {
"font.family": "serif", # use serif/main font for text elements
"text.usetex": True,   # don't setup fonts from rc parameters
"text.latex.preview": True,
"figure.figsize": (10,5),
"pgf.preamble": [
     "\\usepackage{units}",         # load additional packages
     "\\usepackage{metalogo}",
     "\\usepackage{unicode-math}",  # unicode math setup
     r"\setmathfont{xits-math.otf}",
     r"\setmainfont{Arial}", # serif font via preamble
     ]
}
plt.rcParams.update(pgf_with_custom_preamble)

ax = plt.subplot(1, 1, 1)

# get a list for the algorithms and number of users (x axis)
xList = df["number of users"].unique().tolist()
xList = sorted(xList)

algorithms = df["algorithm"].unique().tolist()

# now for each algorithm, we get the mean with respect to number of users, and plot the yvalues
idx = 0
for alg in algorithms:
    yList = []
    yErrs = []

    for x in xList:
        auxDf = df.loc[(df["algorithm"] == alg) & (df["number of users"] == x)]
        yList.append(auxDf.mean().tolist()[0])
        yErrs.append((1.96 / sqrt(len(xList))) * auxDf.std().tolist()[0])

    # plot the means for this algorithm    
    ax.plot(xList, yList, dashes[idx % len(dashes)], fillstyle = "full", color = colors[idx % len(colors)], markersize = 6, marker = markers[idx % len(markers)], label = alg, linewidth = 2.5)

    # plot confidence interval
    ax.errorbar(xList, yList, yerr = yErrs, fmt = "none", ecolor = colors[idx % len(colors)], capsize = 5, capthick = 2, linewidth = 2.5)
    idx += 1

rcParams.update({'font.size': 12})

ax.yaxis.grid(color='gray', linestyle='dashed')
ax.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
ax.legend(loc = "best", ncol = 1)
xticks = [i for i in range(min(xList), max(xList), 10)]
plt.xticks(xticks, xticks)
plt.xlabel("Number Of UEs", fontsize = 18)
plt.ylabel(yname, fontsize=18)
plt.savefig(("../" + filename + '.png'), bbox_inches='tight', pad_inches = 0)
#plt.show()
