# -*- coding: utf-8 -*-
#########################################################
# SERVICE : common.py                                   #
#          Handles common functions                     #
#          I. Helwegen 2024                             #
#########################################################

####################### IMPORTS #########################

#########################################################

####################### GLOBALS #########################

#########################################################

###################### FUNCTIONS ########################

#########################################################

#########################################################
# Class : common                                        #
#########################################################
class common:
    @classmethod
    def getsetting(cls, settings, setting, default = None):
        value = default
        if setting in settings:
            value = settings[setting]
        return value
    
    @classmethod
    def tryImport(cls, module, package = None, root = False, level = 0):
        result = None
        try:
            result = __import__(module, fromlist=[] if root else [""], level=level)
        except ImportError:
            try:
                import pip
                try:
                    if not package:
                        package = module
                    if hasattr(pip, 'main'):
                        pip.main(['install', package])
                    else:
                        pip._internal.main(['install', package])
                except:
                    print("Unable to install required packages")
                    print("{} not installed".format(package))
                    exit(1)
                result = __import__(module, fromlist=[] if root else [""], level=level)
            except:
                print("Pip not installed, please install pip to continue")
                print("Unable to install the required packages")
                exit(1)
        return result
    
    @classmethod
    def Import(cls, module, root = False, level = 0):
        result = None
        try:
            result = __import__(module, fromlist=[] if root else [""], level=level)
        except ImportError:
            pass
        return result
    
    @classmethod
    def gettype(cls, text):
        try:
            retval = int(text)
        except:
            try:
                retval = float(text)
            except:
                if text:
                    if text.lower() == "false":
                        retval = False
                    elif text.lower() == "true":
                        retval = True
                    else:
                        retval = text
                else:
                    retval = ""

        return retval

    @classmethod
    def convnumber(cls, value):
        retval = value
        if type(value) is bool:
            retval = 1 if value else 0
        return retval
    
    @classmethod
    def getUuid(cls, chars = 0):
        myUuid = "1234567890ABCDEF"
        try:
            with open("/etc/machine-id") as f:
                myUuid = f.readline().strip().upper()
        except:
            try:
                with open("/var/lib/dbus/machine-id") as f:
                    myUuid = f.readline().strip().upper()
            except:
                pass
        if chars > 0:
            myUuid = myUuid[:chars]
        elif chars < 0:
            myUuid = myUuid[chars:]
        return myUuid
######################### MAIN ##########################
if __name__ == "__main__":
    pass
