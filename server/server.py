from multiprocessing import Process, Queue
from multiprocessing.process import current_process
from sound import Sound
from motion import Motion
from position import Position
import socket
import time
import serial
import re
import sys
import select

#select code: https://stackoverflow.com/questions/17386487/python-detect-when-a-socket-disconnects-for-any-reason

#receives from serial port all data to be processed
class ArduinoIO():

    def __init__(self, queue):

        self.s = serial.Serial('/dev/ttyUSB1', 9600, timeout=10);
        self.queue = queue

    def run(self):

        try:
            while True:
                line = self.s.readline().decode('utf-8').rstrip('\r\n\s')
                line_ls = line.split(',')

                #makes sure not to include data that was cut off
                if len(line_ls) == 7:
                    data = list(map(lambda x:float(x), line_ls))
                    self.queue.put(data)

                time.sleep(0.01)

        except KeyboardInterrupt:
            print('exiting io')
            client.close()


#calls each processing class to determine position, sound, and motion
class ProcessData():

    def __init__(self, queue):
        
        self.queue = queue
        self.processes = {'position':None, 'motion':None, 'sound':None}
    
    def run(self):

        position = Position()
        motion = Motion()
        sound = Sound()

        try:
            iteration = 0
            while True:

                iteration += 1
                data = self.queue.get()
                #makes sure old data is not processed after most recent
                self.queue.empty()

                rssi = data[:3]
                acc = data[3:6]
                amp = data[6]

                for measure in ['position', 'motion', 'sound']:

                    if self.processes[measure] and not self.processes[measure].is_alive():
                        self.processes[measure] = None

                    if not self.processes[measure]:
                        if measure == 'position':
                            pos_process = Process(target=position.run, args=(rssi, acc))
                            pos_process.start()
                            self.processes[measure] = pos_process

                        elif measure == 'motion':
                            motion_process = Motion(target=motion.run, args=(position.pos,))
                            motion_process.start()
                            self.processes[measure] = motion_process

                        else:
                            sound_process = Sound(target=sound.run, args=(amp,))
                            sound_process.start()
                            self.processes[measure] = sound_process
                
                if iteration > 20:
                    self.send_data(position.curr_pos[0], position.curr_pos[1], motion.curr_speed_type, sound.curr_volume_type)

        except KeyboardInterrupt:
            print('exiting processing')
            client.close()


    def send_data(self, x, y, speed, volume):
        client.send('%.2f,%.2f,%s,%s'.format(x, y, speed, volume))

if __name__=='__main__':

    print('Server On')

    sock = None
    client = None
    #establish connection with android device
    while True:
        try:
            with socket.socket() as sock:
                host = "192.168.0.112"
                port = 50000
                sock.bind((host, port))

                print('Searching for client')
                sock.listen()
                client, addr = sock.accept()
                print('Connected to client')
                
                with client:
                    
                    # while True:
                    #     client.send(b"hello there\r\n");
                    #     time.sleep(100);
                    

                    #shared data queue between io and processer
                    queue = Queue()

                    #run io and processer in parallel
                    arduino_io = ArduinoIO(queue)
                    process_data = ProcessData(queue)
                    serial_io = Process(target=arduino_io.run)
                    serial_data = Process(target=process_data.run)

                    serial_io.start()
                    serial_data.start()

                    try:
                        serial_io.join()
                        serial_data.join()

                    except Exception:
                        print('Disconnected')
                        serial_io.terminate()
                        serial_data.terminate()
                        client.close()

        except KeyboardInterrupt:
            sys.exit(0)
