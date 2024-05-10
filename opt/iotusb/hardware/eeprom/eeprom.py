# -*- coding: utf-8 -*-
#########################################################
# SERVICE : eeprom.py                                   #
#           Class handling EEPROM read/ write/ check    #
#           crc16 checksum creation etc.                #
#           I. Helwegen 2024                            #
#########################################################

####################### IMPORTS #########################
import struct
from common.common import common
import hardware.eeprom.items as items
crc = common.tryImport("crc", "crc")
#########################################################

####################### GLOBALS #########################

#########################################################

###################### FUNCTIONS ########################

#########################################################

#########################################################
# Class : eeprom                                        #
#########################################################
class eeprom(object):
    def __init__(self, type, rev, settings):
        self.data = None
        dataImport = common.Import("hardware.eeprom." + type + "_" + str(rev))
        if dataImport:
            self.data = dataImport.eepromData()
        self.settings = settings

    def __del__(self):
        pass

    def toBytes(self):
        bdata = ""
        if self.data:
            for item in self.data.mem:
                value = 0
                if common.getsetting(self.settings, item.name) != None:
                    value = common.getsetting(self.settings, item.name)
                else:
                    value = item.default
                if item.datatype == items.DT_BYTE:
                    bdata += self.byte2bytes(value)
                elif item.datatype == items.DT_SHORT:
                    bdata += self.short2bytes(value)
                elif item.datatype == items.DT_LONG:
                    bdata += self.long2bytes(value)
                elif item.datatype == items.DT_FLOAT:
                    bdata += self.float2bytes(value)
                elif item.datatype == items.DT_STRING:
                    bdata += self.string2bytes(value, item.length)
        return bdata
    
    def debugData(self, data):
        bdata = ""
        cnt = 0
        if self.data:
            for item in self.data.mem:
                value = 0
                try:
                    if item.datatype == items.DT_BYTE:
                        value = self.bytes2byte(data[2*cnt:2*(cnt+1)])
                        cnt += 1
                    elif item.datatype == items.DT_SHORT:
                        value = self.bytes2short(data[2*cnt:2*(cnt+2)])
                        cnt += 2
                    elif item.datatype == items.DT_LONG:
                        value = self.bytes2long(data[2*cnt:2*(cnt+4)])
                        cnt += 4
                    elif item.datatype == items.DT_FLOAT:
                        value = self.bytes2float(data[2*cnt:2*(cnt+4)])
                        cnt += 4
                    elif item.datatype == items.DT_STRING:
                        value = self.bytes2string(data[2*cnt:2*(cnt+item.length)], item.length)
                        cnt += item.length
                except:
                    pass
                bdata += item.name + ": " + str(value) + "\n"
        return bdata

    def calcCRC(self, data):
        try:
            calculator = crc.Calculator(crc.Crc16.XMODEM)
        except: # versions < 7.0.0
            calculator = crc.Calculator(crc.Crc16.CCITT)
        return calculator.checksum(bytes.fromhex(data))

    def byte2bytes(self, value):
        return struct.pack('B', value).hex().upper()

    def short2bytes(self, value):
        return struct.pack('>H', value).hex().upper()

    def long2bytes(self, value):
        return struct.pack('>L', value).hex().upper()

    def float2bytes(self, value):
        return struct.pack('<f', value).hex().upper() # little endian floar float?

    def string2bytes(self, value, length = items.STRLEN):
        result = ""
        cnt = 0
        
        while cnt < length:
            if cnt < len(value):
                result += struct.pack('B', ord(value[cnt])).hex().upper()
            else:
                result += "00"
            cnt += 1

        return result
    
    def bytes2byte(self, hexval):
        return struct.unpack('B', bytes.fromhex(hexval))[0]
    
    def bytes2short(self, hexval):
        return struct.unpack('>H', bytes.fromhex(hexval))[0]

    def bytes2long(self, hexval):
        return struct.unpack('>L', bytes.fromhex(hexval))[0]

    def bytes2float(self, hexval):
        return struct.unpack('<f', bytes.fromhex(hexval))[0]

    def bytes2string(self, hexval, length = items.STRLEN):
        result = ""
        cnt = 0
        c = 1
        
        while (cnt < length) and c:
            c = struct.unpack('B', bytes.fromhex(hexval[2*cnt:2*(cnt+1)]))[0]
            if c:
                result += chr(c)
        return result

######################### MAIN ##########################
if __name__ == "__main__":
    pass
