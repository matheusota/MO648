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
    df.loc[i] = [line[0], int(line[1]), float(line[2])]
    i += 1

# set some plot parameters
dashes = ['-', ':', '-.', '--']
markers = ['o', 's', '^', 'p', 'h']
colors = ['blue', 'red', 'gray', 'green', 'brown']

pgf_with_custom_preamble = {
"font.family": "serif", # use serif/main font for text elements
"text.usetex": True,   # don't setup fonts from rc parameters
"text.latex.preview": True,
"figure.figsize": (7,5),
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

algorithms = df["algorithm"].unique().tolist()

# now for each algorithm, we get the mean with respect to the desired measure
histValues = []
histNames = []
idx = 0
for alg in algorithms:
    auxDf = df.loc[(df["algorithm"] == alg)]

    histValue = auxDf.mean().tolist()[0]
    histName = alg

    # plot the means for this algorithm    
    ax.bar(histName, histValue, color = colors[idx], width = 0.5)

    idx += 1

rcParams.update({'font.size': 12})

ax.yaxis.grid(color='gray', linestyle='dashed')
ax.yaxis.set_major_formatter(FormatStrFormatter('%.1f'))
plt.xlabel("number of users", fontsize = 18)
plt.ylabel(yname, fontsize=18)
plt.savefig(("../" + filename + '.png'), bbox_inches='tight', pad_inches = 0)
#plt.show()
