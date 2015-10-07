#
# tega header for python
#

# UNIX domain socket address
ADDRESS = '/var/run/tega.sock'

# CRUD OPERATIONS
INSERT= 0 << 14
SELECT = 1 << 14
UPDATE = 2 << 14
DELETE = 3 << 14

# V_REPEAT
SCALAR  = 0 << 12
SET     = 1 << 12
LIST    = 2 << 12

# V_KV
KEYS    = 0 << 11
VALUES  = 1 << 11

# V_BTYPE
B_STRING  = 0b000 << 8
B_BOOLEAN = 0b001 << 8
B_INT     = 0b010 << 8
B_LONG    = 0b011 << 8
B_FLOAG   = 0b100 << 8
B_DOUBLE  = 0b101 << 8

OPERATIONS = lambda _ope, _v_repeat, _v_kv, _v_btype: _ope|_v_repeat|_v_kv|_v_btype

crud = {"select": SELECT, "insert": INSERT, "update": UPDATE, "delete": DELETE}
CRUD = [i+' ' for i in crud]
_pattern = '^({0})\s+([\.\w]+)\s+(.+)$|^({0})\s+([\.\w]+)$'.format('|'.join(crud.keys()))
import re
cmdline = re.compile(_pattern)

