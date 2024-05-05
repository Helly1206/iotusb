# -*- coding: utf-8 -*-
#########################################################
# SERVICE : device.py                                   #
#           Handles devices for every device added      #
#                                                       #
#           I. Helwegen 2024                            #
#########################################################

####################### IMPORTS #########################
import logging
from common.common import common
from interface.iotserial import iotserial
from hardware.eeprom.eeprom import eeprom
from hardware.topics.topics import topics

#########################################################

####################### GLOBALS #########################
EMPTY      = "<empty>"
INITNONAME = -1
INITNOK    = 0
INITOK     = 1
#########################################################

###################### FUNCTIONS ########################

#########################################################

#########################################################
# Class : device                                        #
#########################################################
class device(iotserial):
    def __init__(self, basename, devname, settings, cbconnect, cbget):
        self.logger = logging.getLogger('{}.device'.format(basename))
        self.devname = devname
        self.settings = settings
        self.cbconnect = cbconnect
        self.cbget = cbget
        self.topics = None
        iotserial.__init__(self, basename, self.connectCallback, self.getCallback)

    def __del__(self):
        if self.topics:
            del self.topics
        iotserial.__del__(self)

    def __repr__(self):
        if self.topics:
            return repr(self.topics)
        else:
            return "Unknown"

    def start(self):
        self.logger.info("running [{}]".format(self.devname))
        iotserial.start(self)

    def terminate(self):
        self.logger.info("terminating [{}]".format(self.devname))
        iotserial.terminate(self)

    def connect(self, ports):
        port = ""
        emptyPort = ""
        for tryPort in ports:
            connected = self.tryConnect(tryPort)
            if connected:
                initOk = self.initDevice()
                if initOk == INITOK:
                    port = tryPort
                    self.logger.info("Connected [{}] @ port {}".format(self.devname, port))
                    break
                elif (initOk == INITNONAME) and not emptyPort:
                    emptyPort = tryPort
        if (not port) and emptyPort:
            connected = self.tryConnect(emptyPort)
            if connected:
                initOk = self.initDevice(True)
                if initOk == INITOK:
                    port = emptyPort 
                    self.logger.info("Connected empty device as [{}] @ port {}".format(self.devname, port))
        return port

    def connectCallback(self, connected):
        self.logger.info("Disconnected [{}]".format(self.devname))
        if self.topics:
            del self.topics
        self.topics = None
        if self.cbconnect:
            self.cbconnect(self.devname, connected)

    def getCallback(self, data):
        if "ERROR" in data.keys():
            self.logger.error("Received error from device {}, {}".format(self.devname,data["ERROR"]))
        if "sub" in data.keys():
            if self.cbget:
                try:
                    key = list(data["sub"].keys())[0]
                    alias = key
                    if self.topics:
                        alias = self.topics.getAlias(key)
                    # {'sub': {'button4': False}}
                    self.cbget(self.devname, alias, data["sub"][key])
                except:
                    pass

    def publish(self, key, value):
        result = False
        topic = key
        if self.topics:
            topic = self.topics.getTopic(key)
        # {“pub”: {"relay0": true}}
        if topic:
            cmd = {"pub": {topic: value}}    
            self.set(cmd)
            result = True
        return result
    
    def getTopics(self):
        if self.topics:
            return self.topics.getTopics()
        return {}

    def initDevice(self, selectEmpty = False):
        initOk = INITNOK
        result = self.command("?")
        if result:
            #{'name': '<empty>', 'type': '4relays', 'rev': 1, 'ecs': 12345}
            try:
                if result["type"].lower() == self.settings["type"].lower():
                    if result["name"] == EMPTY:
                        if selectEmpty:
                            cmd = {"name": self.devname}
                            cmdresult = self.command(cmd)
                            if cmdresult == cmd:
                                initOk = INITOK
                        else:
                            initOk = INITNONAME
                    elif result["name"].lower() == self.devname.lower():
                        initOk = INITOK

                if initOk != INITOK:
                    self.disconnect()
            except:
                initOk == INITNOK    
            
            if initOk == INITOK:
                self.checkClearName()
                self.checkEEPROM(result)
                self.loadTopics(result)
                self.set("sub")
        return initOk
    
    def checkEEPROM(self, dev_result):
        promSettings = common.getsetting(self.settings, "eeprom")
        prom=eeprom(dev_result["type"], dev_result["rev"], promSettings)
        data = prom.toBytes()
        ecs = prom.calcCRC(data)
        if ecs != prom.bytes2short(dev_result["ecs"]):
            self.logger.info("Checksum doesn't match device EEPROM, updating EEPROM")
            # write all data for now
            cmd = {"set": data}
            cmdresult = self.command(cmd)
            if ecs != prom.bytes2short(cmdresult["set"]):
                self.logger.error("Error updating EEPROM")
        del prom

    def loadTopics(self, dev_result):
        aliasSettings = common.getsetting(self.settings, "alias")
        self.topics = topics(dev_result["type"], dev_result["rev"], aliasSettings)

    def checkClearName(self):
        if common.getsetting(self.settings, "clear"):
            self.logger.info("Clearing device name")
            cmd = {"name": ""}
            cmdresult = self.command(cmd)
            if cmdresult["name"] != EMPTY:
                self.logger.error("Device name cannot be cleared")

