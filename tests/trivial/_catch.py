try:
    print x
except NotImplementedError, e:
    print e
except RuntimeError as e:
    print "e is"
    print e

