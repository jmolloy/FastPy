#!/usr/bin/env python

import glob, subprocess, difflib, py_compile, sys

for fn in glob.glob("tests/trivial/_*.py"):
    py_compile.compile(fn)
    x = []
    if len(sys.argv) > 1:
        x = [sys.argv[1]]
    try:
        out = subprocess.check_output(['build/fastpy'] + x + [fn + 'c'], stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError, e:
        out = e.output
    with open("%s.out" % fn) as fd:
        idealout = fd.read()
    if out == idealout:
        print "PASS: %s" % fn
    else:
        d = difflib.Differ()
        c = d.compare(idealout.splitlines(), out.splitlines())
        print "FAIL: %s" % fn
        for line in c:
            print line
