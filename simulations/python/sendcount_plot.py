import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

import scalarplot
import vectorplot

"""
xlabel: numSectors---8, 10, 12
data for per label: no-pred (PLM method) in sendInterval period=1,3,5 pred in rangeMax 1,5,9
"""
# 定义莫兰迪色系的六个颜色（Hex色码）
colors = ['#F4A300', '#1976D2', '#7B1FA2', '#0288D1', '#D32F2F', '#388E3C']
secotrs = [8, 12]
width = 0.2
fig = plt.figure(figsize=(8, 6))
ax = plt.subplot()

for sector in secotrs:
    xlabel = np.linspace(sector - 1, sector + 1, 8)
    xlabel = xlabel[1:-1]
    print(xlabel)

    xlabel_index = 0
    # sector - 8
    name = "SendCount:count"

    # no pred
    sendIntervals = [1, 3, 5]
    runattr_map = {
        "isPred": "false",
        "sendInterval": "1",
        "numNode": "30",
        "numSectors": "12",
    }
    for i, sendInterval in enumerate(sendIntervals):
        runattr_map["sendInterval"] = str(sendInterval)
        runattr_map["numSectors"] = str(sector)
        data_avg = scalarplot.scaExportToNp(name, runattr_map)
        # get link life time
        _, linkLifetimeAvg = vectorplot.vecExportToNp(
            "CurrentLinkNum:vector", runattr_map
        )
        timeavg = np.mean(linkLifetimeAvg)
        ax.bar(
            xlabel[xlabel_index],
            data_avg,
            width,
            edgecolor="black",
            color=colors[i],
            label=f"PLM method, Period={sendInterval}",
        )
        xlabel_index += 1

    # ALBP-D
    runattr_map = {
        "isPred": "true",
        "rangeMultipleMax": "1",
        "numNode": "30",
        "numSectors": "12",
    }
    rangeMultipleMaxs = [1, 5, 9]
    for i, rangeMultipleMax in enumerate(rangeMultipleMaxs):
        runattr_map["rangeMultipleMax"] = str(rangeMultipleMax)
        runattr_map["numSectors"] = str(sector)
        data_avg = scalarplot.scaExportToNp(name, runattr_map)
        _, linkLifetimeAvg = vectorplot.vecExportToNp(
            "CurrentLinkNum:vector", runattr_map
        )
        timeavg = np.mean(linkLifetimeAvg)
        ax.bar(
            xlabel[xlabel_index],
            data_avg,
            width,
            hatch="/////",
            edgecolor="black",
            color=colors[i + 3],
            label=f"ALBP-D method, L={rangeMultipleMax}",
        )
        xlabel_index += 1

# ax.set_xlabel("Number of Sectors")
ax.set_ylabel("Overhead / Packets")

handles, labels = plt.gca().get_legend_handles_labels()

# 去重
unique_handles = []
unique_labels = []
for handle, label in zip(handles, labels):
    if label not in unique_labels:
        unique_labels.append(label)
        unique_handles.append(handle)

plt.legend(unique_handles, unique_labels, loc="upper right", handlelength=2)
# ax.legend()
ax.set_xlim([7, 13])
# ax.set_ylim([0, 1])
ax.set_xticks([8, 10, 12])
ax.set_xticklabels([r"$BW=45 \degree$", r"$BW=36 \degree$", r"$BW=30 \degree$"])
plt.savefig("./simulations/python/fig/overhead.pdf", dpi=350)
# plt.tight_layout()
plt.show()
