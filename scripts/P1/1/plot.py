import matplotlib.pyplot as plt

if __name__ == "__main__":
    x = [0, 1, 2]
    ys = [[]
    ]
    x_axis = "process number (log)"
    y_axis = "running time (s)"
    yticks = None
    xlim = [10, 10**7 + 5]
    ylim = []
    labels = ["version 1", "version 2", "MPI version"]

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
    plt.plot(x, ys[2], "o-", label=labels[2])
    """open the grid"""
    # plt.grid(True)

    plt.legend(loc='upper left', borderaxespad=0.)

    plt.show()
