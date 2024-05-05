# -*- coding: utf-8 -*-
#########################################################
# SERVICE : 4relays_1.py                                #
#           Class containing topic data for             #
#           4relays device, revision 1                  #
#           I. Helwegen 2024                            #
#########################################################

####################### IMPORTS #########################

#########################################################

####################### GLOBALS #########################

#########################################################

###################### FUNCTIONS ########################

#########################################################

#########################################################
# Class : topicData                                     #
#########################################################
class topicData:
    name = "4relays_1"
    sub = ["relay1", "relay2", "relay3", "relay4", "button1", "button2", "button3", "button4"]
    pub = ["relay1", "relay2", "relay3", "relay4"]
    topics = [{"cmd_t": "relay1", "stat_t": "relay1",  "type": "switch",        "dev_cla": ""},
              {"cmd_t": "relay2", "stat_t": "relay2",  "type": "switch",        "dev_cla": ""},
              {"cmd_t": "relay3", "stat_t": "relay3",  "type": "switch",        "dev_cla": ""},
              {"cmd_t": "relay4", "stat_t": "relay4",  "type": "switch",        "dev_cla": ""},
              {"cmd_t": "",       "stat_t": "button1", "type": "binary_sensor", "dev_cla": ""},
              {"cmd_t": "",       "stat_t": "button2", "type": "binary_sensor", "dev_cla": ""},
              {"cmd_t": "",       "stat_t": "button3", "type": "binary_sensor", "dev_cla": ""},
              {"cmd_t": "",       "stat_t": "button4", "type": "binary_sensor", "dev_cla": ""}]
    
######################### MAIN ##########################
if __name__ == "__main__":
    pass
