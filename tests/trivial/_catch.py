try:
    print x
except NotImplementedError, e:
    print "E is "
    print e
    print "Motherfucker"
except RuntimeError:
    print "EEK"

