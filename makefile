###########################################
# Simple Makefile for SonIR program
#
# LiamZ
# 2012-08-01
###########################################

all: executable config

CC=gcc
CXX=g++
COBJS=HID_Utilities/HID_Utilities.o HID_Utilities/IOHIDDevice_.o HID_Utilities/IOHIDElement_.o HID_Event/HID_Event.o CoreEvents.o syshotkeys/syshotkey.o
OBJS=$(COBJS)
CFLAGS=-Wall
LIBS=-framework IOKit -framework CoreFoundation -framework Cocoa -framework Carbon -framework ApplicationServices
EXECUTABLE=SonIR
CONFIG_TARGET=~/Library/Preferences/SonIR.Config.plist

executable: $(OBJS)
	gcc -Wall $^ $(LIBS) -o $(EXECUTABLE) main.c

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

copy:

clean:
	rm $(COBJS) $(EXECUTABLE) $(CONFIG_TARGET)
	
config:
	cp -i Config.plist $(CONFIG_TARGET)