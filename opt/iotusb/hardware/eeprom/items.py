# -*- coding: utf-8 -*-
#########################################################
# SERVICE : items.py                                    #
#           EEPROM data items                           #
#                                                       #
#           I. Helwegen 2024                            #
#########################################################

####################### IMPORTS #########################

#########################################################

####################### GLOBALS #########################
DT_NONE = -1
DT_BYTE   = 0
DT_SHORT  = 1
DT_LONG   = 2
DT_FLOAT  = 3
DT_STRING = 4
STRLEN    = 32
#########################################################

###################### FUNCTIONS ########################

#########################################################

#########################################################
# Class : item                                          #
#########################################################
class item:
    def __init__(self, name = "", datatype = DT_NONE, default = 0, length = 0):
        self.name = name
        self.datatype = datatype
        self.default = default
        self.length = 0
        if length:
            self.length = length
        elif datatype == DT_BYTE:
            self.length = 1
        elif datatype == DT_SHORT:
            self.length = 2
        elif datatype == DT_LONG:
            self.length = 4
        elif datatype == DT_FLOAT:
            self.length = 4
        elif datatype == DT_STRING:
            self.length = STRLEN        

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name

    def name(self):
        return self.name

    def datatype(self):
        return self.datatype

    def default(self):
        return self.default

    def length(self):
        return self.length

######################### MAIN ##########################
if __name__ == "__main__":
    pass
