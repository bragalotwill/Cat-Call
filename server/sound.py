import numpy as np

#if more time, use ml on cat sound dataset to determine mood instead of sound
class Sound():

    def __init__(self):
        self.amps = []
        self.volume_types = {'quiet':[100, 135], 'moderate':[135, 150], 'loud':[150, 200]}
        self.curr_volume = 0
        self.curr_volume_type = ''

    def run(self, amp):

        self.amps.append(amp)

        if len(self.amps) < 10:
            return

        self.amps.remove(0)

        self.curr_volume = np.average(self.amps)

        for type, rng in self.volume_types.items():
            if rng[0] <= self.curr_volume < rng[1]:
                self.curr_volume_type = type

