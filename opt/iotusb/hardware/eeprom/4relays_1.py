# -*- coding: utf-8 -*-
#########################################################
# SERVICE : 4relays_1.py                                #
#           Class containing EEPROM data for            #
#           4relays device, revision 1                  #
#           I. Helwegen 2024                            #
#########################################################

####################### IMPORTS #########################
import hardware.eeprom.items as items

#########################################################

####################### GLOBALS #########################

#########################################################

###################### FUNCTIONS ########################

#########################################################

#########################################################
# Class : eepromData                                    #
#########################################################
class eepromData:
    name = "4relays_1"
    mem = [items.item("Enable0", items.DT_BYTE, 1),
           items.item("Enable1", items.DT_BYTE, 1),
           items.item("Enable2", items.DT_BYTE, 1),
           items.item("Enable3", items.DT_BYTE, 1),
           items.item("Latch0", items.DT_BYTE, 1),
           items.item("Latch1", items.DT_BYTE, 1),
           items.item("Latch2", items.DT_BYTE, 1),
           items.item("Latch3", items.DT_BYTE, 1),
           items.item("Inverse0", items.DT_BYTE, 0),
           items.item("Inverse1", items.DT_BYTE, 0),
           items.item("Inverse2", items.DT_BYTE, 0),
           items.item("Inverse3", items.DT_BYTE, 0),
           items.item("Buttonsw0", items.DT_BYTE, 1),
           items.item("Buttonsw1", items.DT_BYTE, 1),
           items.item("Buttonsw2", items.DT_BYTE, 1),
           items.item("Buttonsw3", items.DT_BYTE, 1),
           items.item("Lowpower", items.DT_BYTE, 0),
           items.item("LowpowerLEDs", items.DT_BYTE, 0)]

######################### MAIN ##########################
if __name__ == "__main__":
    pass
