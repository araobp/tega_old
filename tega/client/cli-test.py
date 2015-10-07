#!/usr/bin/env python
#
#
# Python client for Tega
#

import readline
import yaml
import copy

# Readline-related
#with open('cmd.yaml', 'r') as f:
#    cmd = yaml.load(f.read())
cmdlist = """
a:
    b: _end
    c: _end
    d:
        e: _end
x:
    $:
        z: _end
"""
cmd = yaml.load(cmdlist)
#print cmd

def _flatten(options, path, c):
    if c == '_end':
        options.append(path)
        pass
    else:
        for k in c:
            _path = copy.deepcopy(path)
            _path.append(k)
            _flatten(options, _path, c[k])
        
def flatten(s, c):
    options = []
    parent = '/'
    if len(s) > 0:
        parent = '/{}/'.format('/'.join(s)) 
    for k in c:
        path = [k]
        _flatten(options, path, c[k])
    flattend = []
    for i in options:
        flattend.append(parent+'/'.join(i))
    return flattend 

#print flatten(['xxx', 'yyy'], cmd)

readline.parse_and_bind('tab: complete')
readline.parse_and_bind('set editing-mode emacs')
readline.set_completer_delims('')
def completer(text, state):
    t = text.split('/')
    #print t
    c = cmd
    options = []
    if len(t) > 1:
        i = 0
        while i < len(t):
            i += 1
            s = t[i]
            #print s
            if s in c:
                c = c[s]
                options.append(s)
                #print options
            elif '$' in c:
                c = c['$']
                options.append(s)
            else:
                break 
        options = flatten(options, c) 
        #print options
    else:
        options = flatten([], c)
    if state < len(options):
        #print options[state]
        return options[state]
    else:
        return None
readline.set_completer(completer)


if __name__ == '__main__':

    print ('q to quit')

    while True:

        cmd = raw_input(">>> ")
        if cmd == 'q':
            break
        else:
            print '***' 

