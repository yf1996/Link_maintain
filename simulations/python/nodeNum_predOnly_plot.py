import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import ConnectionPatch


import scalarplot
import vectorplot

"""
xlabel: numSectors---8, 10, 12
data for per label: no-pred (PLM method) in sendInterval period=1,3,5 pred in rangeMax 1,5,9
"""
colors = ["#F4A300", "#1976D2", "#7B1FA2", "#0288D1", "#D32F2F", "#388E3C"]
colors = ["#CC6666", "#D9A656", "#678FCB", "#D28A8A", "#E6B96A", "#7EA8CF"]
markers = ["^", "o", "d"]

secotrs = [8, 10, 12]
fig, ax = plt.subplots(figsize=(8, 6))
# sector - 8
name = "LostNodeNum:vector"

for idx, sector in enumerate(secotrs):
    # ALBP-D
    runattr_map = {
        "isPred": "true",
        "rangeMultipleMax": "1",
        "numNode": "30",
        "numSectors": "8",
    }
    runattr_map["numSectors"] = sector
    rangeMultipleMaxs = [1, 5, 9]
    for i, rangeMultipleMax in enumerate(rangeMultipleMaxs):
        runattr_map["rangeMultipleMax"] = str(rangeMultipleMax)
        timeaxis, data_avg = vectorplot.vecExportToNp(name, runattr_map)
        ax.plot(
            timeaxis,
            data_avg,
            color=colors[i],
            linestyle="--",
            marker=markers[idx],
            markersize=7,
            markevery=10,
            label=f"$BW={360/sector}\degree$, L={rangeMultipleMax}",
        )

ax.set_xlabel("Time / s")
ax.set_ylabel("Number of Link")
ax.set_xlim([0, 1200])


# # legend去重
# plt.sca(ax)
# handles, labels = plt.gca().get_legend_handles_labels()
# unique_handles = []
# unique_labels = []
# for handle, label in zip(handles, labels):
#     if label not in unique_labels:
#         unique_labels.append(label)
#         unique_handles.append(handle)
# plt.legend(unique_handles, unique_labels, loc="upper right")
ax.legend()
plt.grid(True, linewidth=0.5, linestyle="-.")

# plt.tight_layout()
plt.savefig("./simulations/python/fig/numNode_predOnly.pdf", dpi=350)
plt.show()
