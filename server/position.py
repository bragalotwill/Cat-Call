import numpy as np
from scipy.optimize import fsolve

#formulas given by http://worldcomp-proceedings.com/proc/p2016/ESC6026.pdf
class Position():


    def __init__(self):

        #constants
        self.tx_power = [-50.0, -50.0, -50.0] #rssi 1m away from beacon
        self.n = [2.0, 2.0, 2.0] #rssi calibration
        self.beacons = [[0, 0], [100, 0], [100, 100]] #beacon coordinates

        self.rssi_ls = []
        self.acc_ls = []

        self.curr_pos = [0, 0]
    

    def run(self, rssi, acc):

        self.rssi_ls.append(rssi)
        self.acc_ls.append(acc)

        if (len(self.rssi_ls) < 10):
            return
        
        self.rssi_ls.remove(0)
        self.acc_ls.remove(0)

        rssi_avg = np.average(self.rssi_ls)

        X = self.sensor(rssi_avg)
        self.curr_pos = X
        #need gyroscope to apply acceleration kalman filter



    def sensor(self, rssi):
        
        d = []
        for i in range(len(self.beacons)):
            A = self.tx_power[i]
            n = self.n[i]
            d.append(10 ** ((A - rssi[i]) / (10 * n)))
            X = fsolve(self.func, [0, 0], args=(d,))

        return X

    
    def func(self, X, d):

        F = []
        for i in range(len(self.beacons)):
            F.append((self.beacons[i][0] - X[0])**2 + (self.beacons[i][1] - X[1])**2 - d[i]**2)

        return F