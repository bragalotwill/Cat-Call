import numpy as np

class Motion():

    def __init__(self):

        self.last_pos = [0, 0]
        self.speeds = []

        self.curr_speed = 0
        self.curr_speed_type = ''

        self.speed_types = {'Still':([0, 0.005]), 'Stalking':[0.005, 0.1], 'Walking':[0.1, 1], 'Running':[1, 10]}
    
    
    def run(self, pos):

        speed = np.sqrt((pos[0] - self.last_pos[0]) ** 2 + (pos[1] - self.last_pos[1]) ** 2)
        self.last_pos = pos
        self.speeds.append(speed)

        if len(self.speeds) < 10:
            return

        self.speeds.remove(0)

        self.curr_speed = np.average(self.speeds)

        for type, rng in self.speed_types.items():
            if rng[0] <= self.curr_speed < rng[1]:
                self.curr_speed_type = type

