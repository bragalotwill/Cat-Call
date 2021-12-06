import numpy as np
from multiprocessing import Queue

#if more time, use ml on cat sound dataset to determine mood instead of sound
class Sound():

    def __init__(self):
        self.amps = []
        self.volume_types = {'quiet':[100, 135], 'moderate':[135, 150], 'loud':[150, 200]}
        self.curr_volume = 0
        self.curr_volume_type = ''

    def run(self, amp, amp_history, sound, q):

        amp_history.append(amp)

        print(len(amp_history))
        if len(amp_history) < 10:
            q.put('quiet')
            q.put(amp_history)
            return

        amp_history.remove(0)

        self.curr_volume = np.average(amp_history)

        for type, rng in self.volume_types.items():
            if rng[0] <= self.curr_volume < rng[1]:
                self.curr_volume_type = type

        sound = [(self.curr_volume_type)]

        q.put(sound)
        q.put(amp_history)

