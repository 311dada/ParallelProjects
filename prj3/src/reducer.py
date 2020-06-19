import sys

max_temp, min_temp = float('-inf'), float('inf')

# input comes from STDIN
for line in sys.stdin:
    # remove leading and trailing whitespace and brackets
    line = line.strip()[1:-1]

    # parse the input we got from mapper.py
    cur_max, cur_min = line.split(',')

    # convert mapper value (currently a string) to float
    try:
        cur_max = float(cur_max.strip())
        cur_min = float(cur_min.strip())
        max_temp = max(max_temp, cur_max)
        min_temp = min(min_temp, cur_min)
    except ValueError:
        # ignore/discard this line
        continue

print(f"maxmimum temperature: {max_temp}\nminimum temperature: {min_temp}")
