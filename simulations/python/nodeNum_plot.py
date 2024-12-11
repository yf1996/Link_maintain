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


secotrs = [8, 10, 12]
fig, ax = plt.subplots(figsize=(8, 6))
# sector - 8
name = "LostNodeNum:vector"

# no pred
sendIntervals = [1, 3, 5]
runattr_map = {
    "isPred": "false",
    "sendInterval": "1",
    "numNode": "30",
    "numSectors": "8",
}
for sendInterval in sendIntervals:
    runattr_map["sendInterval"] = str(sendInterval)
    timeaxis, data_avg = vectorplot.vecExportToNp(name, runattr_map)
    ax.plot(
        timeaxis,
        data_avg,
        label=f"PLM method, Period={sendInterval}",
    )

# ALBP-D
runattr_map = {
    "isPred": "true",
    "rangeMultipleMax": "1",
    "numNode": "30",
    "numSectors": "8",
}
rangeMultipleMaxs = [1, 5, 9]
for rangeMultipleMax in rangeMultipleMaxs:
    runattr_map["rangeMultipleMax"] = str(rangeMultipleMax)
    timeaxis, data_avg = vectorplot.vecExportToNp(name, runattr_map)
    ax.plot(
        timeaxis,
        data_avg,
        marker="^",
        markersize=7,
        markevery=400,
        label=f"ALBP-D method, L={rangeMultipleMax}",
    )
'''
# 定义局部放大的区域
x_min, x_max = 40, 60
y_min, y_max = 10, 20
# 在主图上画出放大区域的矩形
rect = plt.Rectangle(
    (x_min, y_min),
    x_max - x_min,
    y_max - y_min,
    linewidth=1,
    edgecolor="black",
    facecolor="none",
    linestyle="--",
)
ax.add_patch(rect)
zoom_ax = fig.add_axes(
    [0.6, 0.30, 0.15, 0.3]
)  # [left, bottom, width, height] in figure coordinates

# no pred
# sendIntervals = [1, 3, 5]
runattr_map = {
    "isPred": "false",
    "sendInterval": "1",
    "numNode": "30",
    "numSectors": "8",
}
for sendInterval in sendIntervals:
    runattr_map["sendInterval"] = str(sendInterval)
    timeaxis, data_avg = vectorplot.vecExportToNp(name, runattr_map)
    zoom_ax.plot(
        timeaxis,
        data_avg,
        label=f"PLM method, Period={sendInterval}",
    )

# ALBP-D
runattr_map = {
    "isPred": "true",
    "rangeMultipleMax": "1",
    "numNode": "30",
    "numSectors": "8",
}
# rangeMultipleMaxs = [1, 5, 9]
for rangeMultipleMax in rangeMultipleMaxs:
    runattr_map["rangeMultipleMax"] = str(rangeMultipleMax)
    timeaxis, data_avg = vectorplot.vecExportToNp(name, runattr_map)
    zoom_ax.plot(
        timeaxis,
        data_avg,
        marker="^",
        markersize=7,
        markevery=50,
        label=f"ALBP-D method, L={rangeMultipleMax}",
    )
# 设置子图的坐标范围与标题
zoom_ax.set_xlim(x_min, x_max)
zoom_ax.set_ylim(y_min, y_max)
# 添加箭头连接主图的放大区域与子图
# 箭头1：右上角 -> 左上角
con1 = ConnectionPatch(
    xyA=(x_max, y_max),
    xyB=(0, 1),
    coordsA="data",
    coordsB="axes fraction",
    axesA=ax,
    axesB=zoom_ax,
    arrowstyle="->",
    color="red",
)

# 箭头2：右下角 -> 左下角
con2 = ConnectionPatch(
    xyA=(x_max, y_min),
    xyB=(0, 0),
    coordsA="data",
    coordsB="axes fraction",
    axesA=ax,
    axesB=zoom_ax,
    arrowstyle="->",
    color="black",
)

# 将箭头添加到图形中
fig.add_artist(con1)
fig.add_artist(con2)

'''
# 显示图形

ax.set_xlabel("Time / s")
ax.set_ylabel("Number of Node")
ax.set_xlim([0, 1200])
ax.legend()
# plt.tight_layout()
plt.savefig("./simulations/python/fig/numNode.pdf", dpi=350)
plt.show()
