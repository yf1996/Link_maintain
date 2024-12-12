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
name = "CurrentLinkNum:vector"

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
            markevery=400,
            label=f"$BW={360/sector}\degree$, L={rangeMultipleMax}",
        )

# # 定义局部放大的区域
# x_min, x_max = 40, 60
# y_min, y_max = 200, 400
# # 在主图上画出放大区域的矩形
# rect = plt.Rectangle(
#     (x_min, y_min),
#     x_max - x_min,
#     y_max - y_min,
#     linewidth=1,
#     edgecolor="black",
#     facecolor="none",
#     linestyle="--",
# )
# ax.add_patch(rect)
# zoom_ax = fig.add_axes(
#     [0.6, 0.30, 0.15, 0.3]
# )  # [left, bottom, width, height] in figure coordinates

# # no pred
# # sendIntervals = [1, 3, 5]
# runattr_map = {
#     "isPred": "false",
#     "sendInterval": "1",
#     "numNode": "30",
#     "numSectors": "8",
# }
# for i, sendInterval in enumerate(sendIntervals):
#     runattr_map["sendInterval"] = str(sendInterval)
#     timeaxis, data_avg = vectorplot.vecExportToNp(name, runattr_map)
#     zoom_ax.plot(
#         timeaxis,
#         data_avg,
#         linestyle="--",
#         color=colors[i],
#         label=f"PLM method, Period={sendInterval}",
#     )

# # ALBP-D
# runattr_map = {
#     "isPred": "true",
#     "rangeMultipleMax": "1",
#     "numNode": "30",
#     "numSectors": "8",
# }
# # rangeMultipleMaxs = [1, 5, 9]
# for i, rangeMultipleMax in enumerate(rangeMultipleMaxs):
#     runattr_map["rangeMultipleMax"] = str(rangeMultipleMax)
#     timeaxis, data_avg = vectorplot.vecExportToNp(name, runattr_map)
#     print(data_avg)
#     zoom_ax.plot(
#         timeaxis,
#         data_avg,
#         color=colors[i + 3],
#         marker="^",
#         markersize=7,
#         markevery=50,
#         label=f"ALBP-D method, L={rangeMultipleMax}",
#     )
# # 设置子图的坐标范围与标题
# zoom_ax.set_xlim(x_min, x_max)
# zoom_ax.set_ylim(y_min, y_max)
# # 添加箭头连接主图的放大区域与子图
# # 箭头1：右上角 -> 左上角
# con1 = ConnectionPatch(
#     xyA=(x_max, y_max),
#     xyB=(0, 1),
#     coordsA="data",
#     coordsB="axes fraction",
#     axesA=ax,
#     axesB=zoom_ax,
#     arrowstyle="->",
#     color="red",
# )

# # 箭头2：右下角 -> 左下角
# con2 = ConnectionPatch(
#     xyA=(x_max, y_min),
#     xyB=(0, 0),
#     coordsA="data",
#     coordsB="axes fraction",
#     axesA=ax,
#     axesB=zoom_ax,
#     arrowstyle="->",
#     color="black",
# )

# # 将箭头添加到图形中
# fig.add_artist(con1)
# fig.add_artist(con2)

# # 显示图形

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
plt.savefig("./simulations/python/fig/numLink_predOnly.pdf", dpi=350)
plt.show()
