import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import ConnectionPatch

# 数据生成
x = np.linspace(0, 10, 500)
y1 = np.sin(x) + 0.1 * x
y2 = np.sin(x + 0.5) + 0.1 * x

# 创建主图
fig, ax = plt.subplots(figsize=(8, 6))
ax.plot(x, y1, label='Curve 1', color='blue')
ax.plot(x, y2, label='Curve 2', color='orange')
ax.legend()
ax.set_title('Main Plot with Detail Zoom')
ax.set_xlabel('X-axis')
ax.set_ylabel('Y-axis')

# 定义局部放大的区域
x_min, x_max = 4, 6
y_min, y_max = 0.5, 1.5

# 在主图上画出放大区域的矩形
rect = plt.Rectangle((x_min, y_min), x_max - x_min, y_max - y_min,
                     linewidth=1, edgecolor='red', facecolor='none', linestyle='--')
ax.add_patch(rect)

# 创建局部放大的子图
zoom_ax = fig.add_axes([0.5, 0.5, 0.35, 0.35])  # [left, bottom, width, height] in figure coordinates
zoom_ax.plot(x, y1, label='Curve 1', color='blue')
zoom_ax.plot(x, y2, label='Curve 2', color='orange')

# 设置子图的坐标范围与标题
zoom_ax.set_xlim(x_min, x_max)
zoom_ax.set_ylim(y_min, y_max)
zoom_ax.set_title('Zoomed Region')

# 子图添加坐标轴标注
zoom_ax.set_xlabel('X (Zoomed)')
zoom_ax.set_ylabel('Y (Zoomed)')

# 添加箭头连接主图和局部放大图
con1 = ConnectionPatch(xyA=(x_min, y_min), xyB=(0, 0), coordsA="data", coordsB="axes fraction",
                       axesA=ax, axesB=zoom_ax, arrowstyle="->", color="red")
con2 = ConnectionPatch(xyA=(x_max, y_max), xyB=(1, 1), coordsA="data", coordsB="axes fraction",
                       axesA=ax, axesB=zoom_ax, arrowstyle="->", color="red")
fig.add_artist(con1)
fig.add_artist(con2)

# 显示图形
plt.tight_layout()
plt.show()
