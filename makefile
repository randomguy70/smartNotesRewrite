# ----------------------------
# Makefile Options
# ----------------------------

NAME ?= NOTES
ICON ?= icon.png
DESCRIPTION ?= "A User-friendly text editor, v1.0"
COMPRESSED ?= YES
ARCHIVED ?= YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

include $(shell cedev-config --makefile)
#------------------------------
