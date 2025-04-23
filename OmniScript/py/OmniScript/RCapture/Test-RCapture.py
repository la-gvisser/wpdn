# =============================================================================
#    Test-RCapture.py
#        Main project file.
# =============================================================================
#    Copyright (c) WildPackets, Inc. 2014. All rights reserved.

import time
import RCapture

verbose = 1

# [-e <engine IP>]   : IP address of engine. Default is localhost.
# [-p <engine Port>] : Port number of the engine. Default is 6367.
# [-t <auth>]        : Authentication type: 'Default' or 'Third Party'. Default is 'Default'.
# [-d <domain]       : Domain of the credentials.
# [-u <username>     : User account name.
# [-w <password>]    : User account password.

w_cred = ['-e', 'localhost', '-t', 'Default']
l_cred = ['-e', '192.168.96.128', '-t', 'Third Party', '-u', 'gary', '-w', 'wildpackets']
cr = w_cred

# List the adapters
if not RCapture.main(cr+["-l", "-v", verbose]): exit(-1)

# List the filters
if not RCapture.main(cr+["-y", "-v", verbose]): exit(-1)

# List the captures
if not RCapture.main(cr+["-g", "-v", verbose]): exit(-1)

# Delete the filters '_python' and _python-bpf.
RCapture.main(cr+["-j", "_python", "-v", verbose])
RCapture.main(cr+["-j", "_python-bpf", "-v", verbose])

# Create a simple address filter
if not RCapture.main(cr+["-i" "_python:1.2.3.4", "-v", verbose]): exit(-1)

# Create a simple BPF/TCP Dump filter
if not RCapture.main(cr+["-I" "_python-bpf:port(8080) or port(80)", "-v", verbose]): exit(-1)

# Delete the capture 'Python Capture'
# Don't exit if the capture does not exist.
RCapture.main(cr+["-c", "delete", "-n", "Python Capture", "-v", verbose])

# Create the capture 'Python Capture'
if not RCapture.main(cr+["-c", "create", "-a", "0", "-n", "Python Capture", "-b", "128", "-v", verbose]): exit(-1)

# List the properties of the capture 'Python Capture'
if not RCapture.main(cr+["-g", "-n", "Python Capture", "-v", verbose]): exit(-1)

# Start the capture 'Python Capture'
if not RCapture.main(cr+["-c", "start", "-n", "Python Capture"]): exit(-1)

time.sleep(10)

# Stop the capture 'Python Capture'
if not RCapture.main(cr+["-c", "stop", "-n", "Python Capture"]): exit(-1)

# Save the packets from the capture 'Python Capture' to the file 'PythonCapture.pkt'.
if not RCapture.main(cr+["-c", "save", "-n", "Python Capture", "-o", "PythonCapture", "--format", "pkt", "-v", verbose]): exit(-1)

# Get and display the Node Statistics.
if not RCapture.main(cr+["-s", "node", "-n", "Python Capture", "-v", verbose]): exit(-1)

# Get and display the Protocol Statistics.
if not RCapture.main(cr+["-s", "protocol", "-n", "Python Capture", "-v", "1"]): exit(-1)

# Get and display the Summary Statistics.
if not RCapture.main(cr+["-s", "summary", "-n", "Python Capture", "-v", verbose]): exit(-1)

# List packets for capture 'Python Capture'
if not RCapture.main(cr+["-k", "-n", "Python Capture", "-v", verbose]): exit(-1)

# List the packets for all captures.
if not RCapture.main(cr+["-k", "-v", verbose]): exit(-1)

# Delete the capture 'Python Capture'
if not RCapture.main(cr+["-c", "delete", "-n", "Python Capture", "-v", verbose]): exit(-1)

#args = ["-z", "-e", "10.4.2.105", "-t", "Third Party", "-u", "root", "-w", "wildpackets", "-v", verbose] # delete all capture sessions
#args = ["-c", "create", "-a", "0", "-n", "Python Capture", "-b", "128", "-i", "_python:10.4.3.101", "-v", verbose]
#args = ["-c", "create", "-a", "0", "-n", "Python Capture", "-b", "128", "-h", "_python" "-v", verbose]
#args = ["-c", "create", "-q", "Local Area Connection", "-n", "Python Capture", "-b", "123", "-i", "_python:1.2.3.4", "-o", "PyCapt.xml", "-v", verbose]
#args = ["-c", "create", "-q", "Local Area Connection", "-n", "Python Capture", "-b", "123", "-h", "_python", "-o", "PyCapt.xml", "-v", verbose]
##No imp. args = ["-s", "flow", "-n", "Python Capture", "-v", verbose]
#args = ["-?"]
