import pandas as pd

data = pd.read_csv("../csv_data/London2013.csv")
maxm = data.max()[2]
minm = data.min()[2]

data = pd.read_csv("../csv_data/Mumbai2013.csv")
maxm = max(data.max()[2], maxm)
minm = min(data.min()[2], minm)

data = pd.read_csv("../csv_data/Mumbai2013.csv")
maxm = max(data.max()[2], maxm)
minm = min(data.min()[2], minm)

data = pd.read_csv("../csv_data/NewYork2013.csv")
maxm = max(data.max()[2], maxm)
minm = min(data.min()[2], minm)

data = pd.read_csv("../csv_data/SFO2012.csv")
maxm = max(data.max()[2], maxm)
minm = min(data.min()[2], minm)

data = pd.read_csv("../csv_data/SFO2013.csv")
maxm = max(data.max()[2], maxm)
minm = min(data.min()[2], minm)

print(f"maxmimum temperature: {maxm}\nminimum temperature: {minm}")