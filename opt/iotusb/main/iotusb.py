#!/usr/bin/python3

# -*- coding: utf-8 -*-
#########################################################
# SCRIPT : iotusb.py                                    #
#          Service to communicate with IOTrelayUSB      #
#          And interface with MQTT or REST API          #
#          I. Helwegen 2024                             #
#########################################################

####################### IMPORTS #########################
import sys
from threading import Event, Lock
from time import sleep
import signal
import logging
import logging.handlers
import locale
from common.common import common
import yaml
import serial.tools.list_ports as list_ports
from interface.restapi import restapi
from interface.mqtt import mqtt
from hardware.device import device

#########################################################

####################### GLOBALS #########################

APP_NAME = "iotusb"
VERSION = "0.9.2"
YML_FILE = "/etc/iotusb.yml"
PORTSNAMES = ["arduino", "pro micro"]

#########################################################

###################### FUNCTIONS ########################

#########################################################
# Class : iotusb                                        #
#########################################################
class iotusb(object):
    def __init__(self):
        self.settings = {}
        self.updater = None
        self.term = Event()
        self.term.clear()
        self.devices = {}
        self.mutex = Lock()
        self.logger = logging.getLogger(APP_NAME)
        self.logger.setLevel(logging.DEBUG)
        ch = logging.StreamHandler(sys.stdout)
        self.logger.addHandler(ch)
        locale.setlocale(locale.LC_TIME,'')
        strformat=("{} {}".format(locale.nl_langinfo(locale.D_FMT),locale.nl_langinfo(locale.T_FMT)))
        strformat=strformat.replace("%y", "%Y")
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(name)s - %(message)s', strformat)
        ch.setFormatter(formatter)
        self.logger.info("{} app, version {}".format(APP_NAME, VERSION))

    def __del__(self):
        del self.mutex
        del self.term
        del self.devices
        del self.settings

    def run(self, argv):
        signal.signal(signal.SIGINT, self.exit_app)
        signal.signal(signal.SIGTERM, self.exit_app)
        self.handleArgs(argv)
        self.setlogger()
        
        self.initDevices()
        for devname, data in self.devices.items():
            data["status"]["device"] = device(APP_NAME, devname, data["settings"], self.connectedcb, self.getcb)
            data["status"]["device"].start()
        self.restapi=restapi(self, APP_NAME, common.getsetting(self.settings, "restapi"), list(self.devices.keys()))
        self.restapi.start()
        self.mqtt=mqtt(self, APP_NAME, common.getsetting(self.settings, "mqtt"))
        retry = self.mqtt.connect()
        initial = True
        while not self.term.is_set():
            try:
                if not self.allConnected():
                    ports = self.getPorts()
                    for devname, data in self.devices.items():
                        if not data["status"]["port"]:
                            if data["status"]["device"]:
                                port = data["status"]["device"].connect(ports)
                                with self.mutex:
                                    data["status"]["port"] = port
                                if port in ports:
                                    ports.remove(port)
                                if port:
                                    self.mqtt.add(devname, data["status"]["device"].getTopics())
                                if initial and not port:
                                    self.logger.info("Device not found: {}".format(devname))
                initial = False
            except:
                pass
            sleep(1)
            retry = self.mqtt.connect(retry) # reconnect if connection failed the first time

    def handleArgs(self, argv):
        if len(sys.argv) > 1:
            arg = sys.argv[1]
            if arg == "-h" or arg == "--help":
                self.printHelp()
            else:
                self.logger.error("Incorrect argument entered")
                self.printError()
        try:
            with open(YML_FILE, "r") as f:
                try:
                    self.settings = yaml.safe_load(f)
                except:
                    self.logger.error("Error parsing yaml file")
                    self.printError()
        except:
            pass

    def setlogger(self):
        settingsGeneral = common.getsetting(self.settings, "general")
        if settingsGeneral:
            settingsLogging = common.getsetting(settingsGeneral, "logging")
            if settingsLogging:
                if settingsLogging.lower() == "error":
                    self.logger.setLevel(logging.ERROR)
                elif settingsLogging.lower() == "info":
                    self.logger.setLevel(logging.INFO)    
    
    def getPorts(self):
        ports = []
        for p in list_ports.comports():
            for name in PORTSNAMES:
                if name in p.description.lower():
                    ports.append(p.device)
                    break   
        # remove ports that are already connected from list
        try:
            for devname, data in self.devices.items():
                port = data["status"]["port"]
                if port in ports:
                    ports.remove(port)        
        except:
            pass
        return ports
    
    def set(self, devname, key, value):
        retval = None
        self.logger.debug(">" + devname + ": " + key + " = " + str(value))
        try:
            with self.mutex:
                if self.devices[devname]["status"]["port"]:
                    if self.devices[devname]["status"]["device"].publish(key, value):
                        retval = value
        except:
            pass
        return retval

    def loadModel(self, devname):
        return repr(self.devices[devname]["status"]["device"])
    
    def initDevices(self):
        self.devices = {}
        devicesInfo = common.getsetting(self.settings, "devices")
        try:
            for devname, settings in devicesInfo.items():
                self.devices[devname] = {}
                self.devices[devname]["settings"] = settings
                self.devices[devname]["status"] = {"device": None, "port": ""}
        except:
            pass
        return
    
    def allConnected(self):
        result = True
        try:
            for devname, data in self.devices.items():
                if not data["status"]["port"]:
                    result = False
                    break        
        except:
            result = False
        return result

    def connectedcb(self, devname, connected):
        if not connected:
            with self.mutex:
                self.devices[devname]["status"]["port"] = ""

    def getcb(self, devname, key, value):
        self.logger.debug("<" + devname + ": " + key + " = " + str(value))
        self.restapi.setValue(devname, key, value)
        self.mqtt.setValue(devname, key, value)

    def printHelp(self):
        print("Option:")
        print("    -h, --help: print this help file and exit")
        print(" ")
        print("Enter options in /etc/iotrelayusb.yml")
        exit(0)

    def printError(self):
        print("Enter {} -h for help".format(APP_NAME))
        exit(1)

    def exit_app(self, signum, frame):
        print("Exit app")
        self.term.set()
        for devname, data in self.devices.items():
            if data["status"]["device"]:
                data["status"]["device"].terminate()
                data["status"]["device"].join(5)
        if self.mqtt != None:
            self.mqtt.terminate()
        if self.restapi != None:
            self.restapi.terminate()
            self.restapi.join(5)

#########################################################

######################### MAIN ##########################
if __name__ == "__main__":
    iotusb().run(sys.argv)