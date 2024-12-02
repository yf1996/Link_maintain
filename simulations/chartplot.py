import pandas as pd
from omnetpp.scave import results, chart, utils
import math
import json
import numpy as np
import matplotlib.pyplot as plt

folderPath = "./simulations/results/"
filePath = "1.csv"

# with open(folderPath + filePath, "r", encoding="utf-8") as file:
#     data = json.load(file)  # 将 JSON 文件内容解析为 Python 对象（通常是字典或列表）
#     print(data['LinkMaintain-0-20241128-14:13:30-20020']['vectors'][1].keys())

df = pd.read_csv(folderPath + filePath)
print(df.iloc[:, 1::2])
df_data = df.iloc[:, 1::2]
df_data_1 = df_data.iloc[:, :10].values
print(df_data_1)
df_data_2 = df_data.iloc[:, 10:].values
print(df_data_2)

data_sector_6 = np.mean(df_data_1, axis=1)
data_sector_8 = np.mean(df_data_2, axis=1)

plt.plot(df.iloc[:, 0].values, data_sector_6, label="6")
plt.plot(df.iloc[:, 0].values, data_sector_8, label="8")
plt.legend()
plt.xlim([0, 1000])
plt.show()
