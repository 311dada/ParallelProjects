import matplotlib.pyplot as plt

if __name__ == "__main__":
    x = [0, 1, 2]
    ys = [[26.596131, 12.800729, 2.899818], [29.319315, 8.234695, 1.086909]
    ]
    x_axis = "process number (log)"
    y_axis = "running time (s)"
    yticks = None
    xlim = [10, 10**7 + 5]
    ylim = []
    labels = ["single host", "4 hosts", "MPI version"]

    fig, ax = plt.subplots()

    plt.xlabel(x_axis)
    plt.ylabel(y_axis)
    """set interval for y label"""
    # ax.set_yticks(yticks)
    # """set min and max value for axes"""
    # ax.set_ylim(xlim)
    # ax.set_xlim(xlim)

    plt.plot(x, ys[0], "x-", label=labels[0])
    plt.plot(x, ys[1], "+-", label=labels[1])
    """open the grid"""
    # plt.grid(True)

    plt.legend(loc='upper left', borderaxespad=0.)

    plt.show()
