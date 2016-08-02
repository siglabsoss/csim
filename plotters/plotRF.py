#!/usr/bin/env python

"""
Plot data received over a ZeroMQ port in real time using matplotlib.

Notes
-----
This appears to segfault when run using the WxAgg backend.
"""

import time
import multiprocessing as mp
from struct import *

import zmq
import numpy as np
import matplotlib
matplotlib.use('GTKAgg')
import matplotlib.pyplot as plt
import sys

SAMPLES_PER_UPDATE = 300000

def plot(radioId, fieldNo):
    """
    Plot received data.
    """
    ctx = zmq.Context()
    sock = ctx.socket(zmq.SUB)
    sock.setsockopt(zmq.SUBSCRIBE, "RF")
    sock.connect('tcp://localhost:5555')

    #poller = zmq.Poller()
    #poller.register(sock, zmq.POLLIN)

    fig = plt.gcf()
    ax = fig.add_subplot(111)
    ax.set_ylim((-20, 20))
    line, = ax.plot([1], [1], linewidth=2)

    count = 0
    draw_count = 0
    X = []
    Y = []
    plt.show(block=False)
    print "Starting Plotter..."
    while True:
        #if is_poll_in(sock, poller):
        buf = sock.recv()
        result = unpack('Idddd', buf[2:])
        if result[0] == radioId:
            Y.append(result[fieldNo])
            X.append(count)
            count = count + 1
            draw_count = draw_count + 1
            #print X
            #print Y
            if draw_count > SAMPLES_PER_UPDATE:
                draw_count = SAMPLES_PER_UPDATE/2
                #print "Plotting"
                X = X[-len(X)/2:]
                Y = Y[-len(Y)/2:]
                line.set_data(X, Y)
                # Adjust the X axis labels to enable scrolling effect as X values
                # increase:
                ax.set_xbound(lower=X[0], upper=X[-1])
                fig.canvas.draw()
                plt.pause(0.00000001)


if __name__ == '__main__':
    if (len(sys.argv) < 3):
        print "Usage: %s radioId fieldNo" % sys.argv[0]
        exit()
    radioId = (int(sys.argv[1]))
    fieldNo = (int(sys.argv[2]))
    plt.ion()
    plot(radioId, fieldNo)

