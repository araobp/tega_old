#
# KVS client
#

"""
NOTE: Python int corresponds to int16_t. However, Tega KVS supports uint16_t only. Thus this client can handle max 0x7fff.

"""

import socket
import struct

# UNIX domain socket address
ADDRESS = '/var/run/tega.sock'

# CRUD OPERATIONS
INSERT = 0 << 14 
SELECT = 1 << 14 
UPDATE = 2 << 14 
DELEET = 3 << 14 

# V_REPEAT
SCALAR  = 0 << 12 
SET     = 1 << 12 
LIST    = 2 << 12 

# V_KV
KEYS    = 0 << 11 
VALUES  = 1 << 11 

# V_BTYPE
B_BOOLEAN = 0b000 << 8
B_STRING  = 0b001 << 8
B_INT     = 0b010 << 8
B_LONG    = 0b011 << 8
B_FLOAG   = 0b100 << 8
B_DOUBLE  = 0b101 << 8

operations = lambda _ope, _v_repeat, _v_kv, _v_btype: _ope|_v_repeat|_v_kv|_v_btype

pack = struct.pack

# PDU in network byte order
def pdu(_ver, _operations, _key, _values): 
    _k_len = len(_key)
    _v_len = len(_values)
    return pack('!HHHH{}s{}s'.format(_k_len, _v_len), _k_len, _ver, _operations, _v_len , _key, _values)


if __name__ == '__main__':
    
    _key = 'idb.root.a.b.c.d'
    _values = 'alice, bob'
    _ver = 0 

    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(ADDRESS)
   
    #while True:
    for i in range(4096):
       #raw_input("Enter to send pdu")
       _ver += 1
       _operations = operations(INSERT, SET, VALUES, B_STRING) 
       _pdu = pdu(_ver, _operations, _key, _values)
       #print _pdu
       s.send(_pdu)
       #print s.recv(4096)
       s.recv(4096)


