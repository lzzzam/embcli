import logging
import functools
import os

def initLog(path):
    # Extract directory name
    dir = os.path.dirname(path)
    
    # Create log directory if it not exist
    if os.path.isdir(dir) == False:
        os.mkdir(dir)
    
    # Setup root logger
    logging.basicConfig(filename=f'{dir}/report.log', 
                        format='[%(levelname)s]:\t\t%(message)s', 
                        filemode='w', 
                        level=logging.INFO)

def startTestLog(test):
    logging.info("--------------------------------------------")
    logging.info(f"START OF TEST: {test._testMethodName}")
    
def stopTestLog(test):
    logging.info(f"END OF TEST: {test._testMethodName}")
    logging.info("--------------------------------------------")


def logCommand(cmd):
    string = ''
    for b in cmd.serialize():
        string += '{:02x} '.format(b)
        
    logging.info(f"<- {string}")

def logResponse(rsp):
    string = ''
    for b in rsp:
        string += '{:02x} '.format(b)
        
    logging.info(f"-> {string}")

def testLogger(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        logging.info("--------------------------------------------")
        logging.info(f"START TEST: {func.__name__}")
        value = func(*args, **kwargs)
        logging.info(f"STOP TEST: {func.__name__}")
        logging.info("--------------------------------------------")
        return value
    return wrapper