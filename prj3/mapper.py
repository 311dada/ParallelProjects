import sys

max_temp, min_temp = float('-inf'), float('inf')
# input comes from STDIN (standard input)
for line in sys.stdin:
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into tokens
    _, time, temperature = line.split(',')
    try:
        temperature = float(temperature.strip())
        max_temp = max(max_temp, temperature)
        min_temp = min(min_temp, temperature)
    except:
        pass
print((max_temp, min_temp))


