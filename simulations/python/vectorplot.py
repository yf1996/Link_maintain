import math
import numpy as np
import pandas as pd
import os
import subprocess
import matplotlib.pyplot as plt
import common_func


# collect parameters for query
"""
type =~ vector 
AND runattr:experiment =~ LinkMaintain 
AND runattr:measurement =~ "$numSectors=*, $rangeMultipleMax=*" 
AND runattr:replication =~ "*" 
AND module =~ "Network.radioMedium" 
AND name =~ CurrentLinkNum:vector
"""


def vecExportToCsv(name, runattr_map):
    # get path
    retval = os.getcwd()
    # print("当前工作目录为 %s" % retval)
    os.chdir(retval + "./simulations/results")
    # print("当前工作目录为 %s" % os.getcwd())

    opp_scavetool_exe = "E:/Workplace/omnetpp/omnetpp-6.0/bin/opp_scavetool.exe export "
    name_filter = f'"{name}"'
    type_filter = "vector"

    # runattr_measurement and output name
    runattr_mesurement = ""
    outputFileName = name.split(":")[0]

    for key, value in runattr_map.items():
        runattr_mesurement += f"${key}={value}*"
        outputFileName += f",{key}-{value}"

    runattr_mesurement = f'\\"' + runattr_mesurement + f'\\"'
    outputFileName = outputFileName + ".csv"

    filter_cmd = (
        "type =~ "
        + type_filter
        + " AND name =~ "
        + name_filter
        + " AND runattr:measurement =~ "
        + runattr_mesurement
    )

    outputPath = retval + "/simulations/results_csv/"

    # outputFileName = f"{runattr_mesurement}.csv"
    run_cmd = (
        opp_scavetool_exe
        + "-f"
        + " '"
        + filter_cmd
        + "' "
        + "-o "
        + outputPath
        + outputFileName
        + " -F CSV-S *.vec *sca"
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


def vecExportToNp(measurement_name, runattr_map):
    filename = vecExportToCsv(name=measurement_name, runattr_map=runattr_map)
    # read data and plot
    df = pd.read_csv(filename)
    timeAxis = df.iloc[:, 0].values
    data_rep = df.iloc[:, 1:-1:2].values
    data_avg = np.mean(data_rep, axis=1)
    timeAxis, data_avg = common_func.remove_duplicates(timeAxis, data_avg)

    return [timeAxis, data_avg]


if __name__ == "__main__":
    # name setting
    measurement_name = "CurrentLinkNum:vector"  # "LostNodeNum"

    # runattr setting
    """
    Warning: runattr measurement name must be same as variable name.
    """
    isPred = False

    numNode = 30

    sendInteval = [1, 2, 3, 4, 5]
    # sendInteval = [4]

    numSectors = [8, 10, 12]  # 8 10 12
    numSectors = [8]  # 8 10 12

    rangeMultipleMaxs = [1, 2, 3, 4, 5, 6]
    # rangeMultipleMaxs = [1]

    ax = plt.subplot()

    for intval in sendInteval:
        for numsector in numSectors:
            runattr_map = {
                "isPred": str(isPred).lower(),
                "sendInterval": str(intval),
                "numNode": str(numNode),
                "numSectors": str(numsector),
            }
            # filename = vecExportToCsv(name=measurement_name, runattr_map=runattr_map)
            # read data and plot
            # df = pd.read_csv(filename)
            # timeAxis = df.iloc[:, 0].values
            # data_rep = df.iloc[:, 1:-1:2].values
            # data_avg = np.mean(data_rep, axis=1)
            timeAxis, data_avg = vecExportToNp(measurement_name, runattr_map)
            ax.plot(timeAxis, data_avg)
            # ax.legend()
    ax.set_xlim([0, 250])
    plt.show()
