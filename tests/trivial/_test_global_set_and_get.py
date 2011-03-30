x = 1

# Must try and confuse the DFG generator into re-loading
# the global dict by creating a new basic block.
if x:
    print x
