# -*- coding: utf-8 -*-
#########################################################
# SERVICE : serial.py                                   #
#           Handles serial communication with IOTUSB    #
#           devices                                     #
#           I. Helwegen 2024                            #
#########################################################

####################### IMPORTS #########################
from threading import Thread, Lock, Event
from json import dumps, loads 
import logging
from time import sleep
from common.common import common
serial = common.tryImport("serial", "pyserial")
#########################################################

####################### GLOBALS #########################
BAUDRATE = 115200
#########################################################

###################### FUNCTIONS ########################

#########################################################

#########################################################
# Class : iotserial                                     #
#########################################################
class iotserial(Thread):
    def __init__(self, basename, connectedcb, getcb):
        self.connectedcb = connectedcb
        self.getcb = getcb
        self.serial = serial.Serial()
        self.mutex = Lock()
        self.term = Event()
        self.term.clear()
        self.connected = False
        Thread.__init__(self)

    def __del__(self):
        del self.term
        del self.mutex
        del self.serial

    def tryConnect(self, port):
        self.serial.baudrate = BAUDRATE
        self.serial.port = port
        self.serial.timeout = 1
        try:
            self.serial.open()
            if self.serial.isOpen():
                self.connected = True
        except:
            self.disconnect()
        return self.connected

    def terminate(self):
        self.term.set()
        with self.mutex:
            if self.serial.isOpen():
                self.serial.close()
            self.connected = False

    def run(self):
        while not self.term.is_set():
            # read/ subscribed never requires a response
            try:
                with self.mutex:
                    if self.connected: 
                        line = self.serial.readline().decode("utf-8").strip("\n")
                        if line and self.getcb:
                            self.getcb(loads(line))
            except serial.serialutil.SerialException:
                self.disconnect(True)
            sleep(0.1)

    def command(self, cmd):
        result = {}
        try:
            cmd = dumps(cmd) + "\n"
            with self.mutex:
                if self.connected: 
                    self.serial.write(cmd.encode("utf-8"))
                    line = self.serial.readline().decode("utf-8").strip("\n")
                    result = loads(line)
        except:
            self.disconnect(True)
            result = {}
        return result

    def set(self, cmd):
        result = False
        try:
            cmd = dumps(cmd) + "\n"
            with self.mutex:
                if self.connected: 
                    self.serial.write(cmd.encode("utf-8"))
                    result = True
        except:
            self.disconnect(True)
            result = False
        return result
    
    def disconnect(self, cb = False):
        with self.mutex:
            if self.serial.isOpen():        
                self.serial.close()
            self.connected = False
        if cb and self.connectedcb:
            self.connectedcb(self.connected)
        
