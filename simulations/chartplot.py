import math
import numpy as np
import pandas as pd
import os
import subprocess
import matplotlib.pyplot as plt


# collect parameters for query
"""
type =~ vector 
AND runattr:experiment =~ LinkMaintain 
AND runattr:measurement =~ "$numSectors=*, $rangeMultipleMax=*" 
AND runattr:replication =~ "*" 
AND module =~ "Network.radioMedium" 
AND name =~ CurrentLinkNum:vector
"""


def exportToCsv(measurement_name, numSectors, rangeMultipleMax):
    # get path
    retval = os.getcwd()
    print("当前工作目录为 %s" % retval)
    os.chdir(retval + "./simulations/results")
    print("当前工作目录为 %s" % os.getcwd())

    # export data to csv
    # numSectors = 12
    # rangeMultipleMax = 1
    opp_scavetool_exe = "E:/Workplace/omnetpp/omnetpp-6.0/bin/opp_scavetool.exe export "
    type_filter = "vector"
    # measurement_name = "CurrentLinkNum"
    name_filter = f'"{measurement_name}:vector"'
    runattr_mesurement = (
        f'\\"$numSectors={numSectors}, $rangeMultipleMax={rangeMultipleMax}\\"'
    )
    filter_cmd = (
        "type =~ "
        + type_filter
        + " AND name =~ "
        + name_filter
        + " AND runattr:measurement =~ "
        + runattr_mesurement
    )
    outputPath = retval + "/simulations/results_csv/"
    outputFileName = f"{measurement_name},numSectors-{numSectors},rangeMultipleMax-{rangeMultipleMax}.csv"
    run_cmd = (
        opp_scavetool_exe
        + "-f"
        + " '"
        + filter_cmd
        + "' "
        + "-o "
        + outputPath
        + outputFileName
        + " -F CSV-S *.vec"
    )
    # print(run_cmd)
    subprocess.run(
        [f"powershell.exe", "-Command", run_cmd],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    os.chdir(retval)
    return outputPath + outputFileName


if __name__ == "__main__":
    # numSectors = [8, 10, 12]  # 8 10 12
    numSectors = [8]  # 8 10 12
    # rangeMultipleMax diff
    rangeMultipleMaxs = [1, 2, 3, 4, 5, 6]
    # rangeMultipleMaxs = [1]
    measurement_name = "LostNodeNum"  # "LostNodeNum"

    ax = plt.subplot()
    # export data
    for rangeMultipleMax in rangeMultipleMaxs:
        for numSector in numSectors:
            filename = exportToCsv(
                measurement_name=measurement_name,
                numSectors=numSector,
                rangeMultipleMax=rangeMultipleMax,
            )
            print(filename)
            # read data and plot
            df = pd.read_csv(filename)
            timeAxis = df.iloc[:, 0].values
            data_rep = df.iloc[:, 1:-1:2].values
            data_avg = np.mean(data_rep, axis=1)
            ax.plot(timeAxis, data_avg, label=f"{filename}")
            ax.legend()
    plt.show()
