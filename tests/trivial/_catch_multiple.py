try:
    print x
except (NotImplementedError, RuntimeError) as e:
    print e
