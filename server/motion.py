import numpy as np
from multiprocessing import Queue

class Motion():

    def __init__(self):

        self.last_pos = [0, 0]
        self.speeds = []

        self.curr_speed = 0
        self.curr_speed_type = ''

        self.speed_types = {'Still':([0, 0.005]), 'Stalking':[0.005, 0.1], 'Walking':[0.1, 1], 'Running':[1, 10]}
    
    
    def run(self, pos, pos_history, speed_history, motion, q):
        
        if len(pos_history) > 0:
            print(pos, pos_history[-1])
            self.last_pos = pos_history[-1]
            speed = np.sqrt((pos[0] - self.last_pos[0]) ** 2 + (pos[1] - self.last_pos[1]) ** 2)
            speed_history.append(speed)
            pos_history.append(speed)

            if len(speed_history) < 10:
                return

            speed_history.remove(0)
            pos_history.remove(0)

            self.curr_speed = np.average(speed_history)

            for type, rng in self.speed_types.items():
                if rng[0] <= self.curr_speed < rng[1]:
                    self.curr_speed_type = type

            motion = [self.curr_speed_type]
        else:
            motion = ['still']
            pos_history.append(pos)
        q.put(motion)
        q.put(speed_history)
        q.put(pos_history)
        
