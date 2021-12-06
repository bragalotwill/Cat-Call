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
                try:
                    line = self.s.readline().decode('utf-8').rstrip('\r\n\s')
                    print(line)
                    line_ls = line.split(',')
                    #makes sure not to include data that was cut off
                    if len(line_ls) == 7:
                        try:
                            data = list(map(lambda x:float(x), line_ls))
                            self.queue.put(data)
                        except Exception as e:
                            print(e)
                            continue
                except Exception as e:
                    continue

                time.sleep(0.01)

        except KeyboardInterrupt:
            print('exiting io')
            client.close()


#calls each processing class to determine position, sound, and motion
class ProcessData():

    def __init__(self, queue, rssi_history, pos_history, vel_history, amp_history):
        
        self.queue = queue
        self.processes = {'position':None, 'motion':None, 'sound':None}
        self.rssi_history = rssi_history
        self.pos_history = pos_history
        self.vel_history = vel_history
        self.amp_history = amp_history

    def run(self):

        position = Position()
        motion = Motion()
        sound = Sound()

        
        pos = []
        mo = []
        volume = []

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
                            q = Queue()
                            pos_process = Process(target=position.run, args=(rssi, acc, self.rssi_history, pos, q))
                            pos_process.start()
                            pos = q.get()
                            self.rssi_history = q.get()

                            self.processes[measure] = pos_process

                        elif measure == 'motion':
                            if len(pos) > 0:
                                q = Queue()
                                print(pos)
                                motion_process = Process(target=motion.run, args=(pos[-1], self.pos_history, self.vel_history, mo, q))
                                motion_process.start()
                                mo = q.get()
                                self.vel_history = q.get()
                                self.pos_history = q.get()
                                self.processes[measure] = motion_process

                        else:
                            q = Queue()
                            sound_process = Process(target=sound.run, args=(amp, self.amp_history, volume, q))
                            sound_process.start()
                            volume = q.get()
                            self.amp_history = q.get()
                            self.processes[measure] = sound_process
               
                if iteration > 20 and iteration % 100 == 0 and len(pos) > 0 and len(mo) > 0 and len(volume) > 0:
                    self.send_data(pos[-1][0], pos[-1][1], motion[-1], volume[-1])

        except KeyboardInterrupt:
            print('exiting processing')
            client.close()


    def send_data(self, x, y, speed, volume):
        data = ''+str(x)+','+str(y)+','+speed+','+ volume+'\r\n'
        print('sending', data)
        client.send(bytes(data, encoding='utf-8'))

if __name__=='__main__':

    print('Server On')

    sock = None
    client = None
    #establish connection with android device
    while True:
        with socket.socket() as sock:
            try:
                host = "192.168.0.112"
                port = 50000
                sock.bind((host, port))
                sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                

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

        
                    rssi_history = []
                    pos_history = []
                    amp_history = []
                    vel_history = []
                    
                    #run io and processer in parallel
                    arduino_io = ArduinoIO(queue)
                    process_data = ProcessData(queue, rssi_history, pos_history, amp_history, vel_history)
                    serial_io = Process(target=arduino_io.run)
                    serial_data = Process(target=process_data.run)

                    serial_io.start()
                    serial_data.start()

                    try:
                        serial_io.join()
                        serial_data.join()

                    except Exception as e:
                        print('Disconnected')
                        print(e)
                        serial_io.terminate()
                        serial_data.terminate()
                        client.close()
                        sys.exit(0)
            
            except KeyboardInterrupt:
                print('Finished')
                if client:
                    client.close()
                if sock:
                    sock.close()
                sys.exit(0)

            except Exception as e:
                print('Disconnected')
                print(e)
                sys.exit(0)
                if sock:
                    sock.close()
                if client:
                    client.close()
            

