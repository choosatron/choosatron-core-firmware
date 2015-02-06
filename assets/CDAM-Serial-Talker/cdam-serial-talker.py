#!/usr/bin/env python

import subprocess
import argparse
import serial
import struct
import glob
import sys
import os

from modem import YMODEM

kCdamSerial = None

CMD_START = chr(0x63)
CMD_ADD_STORY = chr(0x08)
CMD_REMOVE_STORY = chr(0x09)
CMD_REMOVE_ALL_STORIES = chr(0x0A)

class CDAMSerial(object):

	def __init__(self):
		global kCdamSerial
		parser = argparse.ArgumentParser(
			description='Choosatron Serial Talker',
			usage='''cdam-serial-talker <command> [<args>]

Choosatron Serial Commands:
   addStory     Add a story to the Choosatron
''')
		parser.add_argument('command', help='Subcommand to run')
		# parse_args defaults to [1:] for args, but you need to
		# exclude the rest of the args too, or validation will fail
		args = parser.parse_args(sys.argv[1:2])
		if not hasattr(self, args.command):
			print 'Unrecognized command'
			parser.print_help()
			exit(1)

		# Serup Serial connection
		serialPaths = glob.glob("/dev/cu.usb*")
		if len(serialPaths) > 0:
			kCdamSerial = serial.Serial(serialPaths[0], 9600, timeout=5, writeTimeout=5)
			# use dispatch pattern to invoke method with same name
			getattr(self, args.command)()
		else:
			print "No USB Serial Device Found"

	def getc(size, timeout=1):
		global kCdamSerial
		return kCdamSerial.read(size) or None

	def putc(data, timeout=1):
		global kCdamSerial
		return kCdamSerial.write(data) or None

	def addStory(self):
		global kCdamSerial
		parser = argparse.ArgumentParser(
			description='Add a story to the Choosatron')
		# prefixing the argument with -- means it's optional
		# NOT prefixing the argument with -- means it's not optional
		parser.add_argument('filename', action='store')
		parser.add_argument('--index', action='store', type=int, default=20)

		args = parser.parse_args(sys.argv[2:])
		print 'Filename=%s' % args.filename
		print 'Index=%d' % args.index
		filesize = os.path.getsize(args.filename)
		print "Binary filesize: " + str(filesize)
		sizeBytes = bytearray(struct.pack('>L', filesize))

		data = bytearray()

		#serialPath = subprocess.check_output('ls /dev/cu.usb*', shell=True);
		#print serialPath

		data += CMD_START
		data += CMD_ADD_STORY
		data += sizeBytes
		data += chr(args.index)
		kCdamSerial.write(data)

		try:
			ymodem = YMODEM(getc, putc)
			status = ymodem.send([filename])
		except:
			print "Unexpected error:", sys.exc_info()[0]


	def removeStory(self):
		parser = argparse.ArgumentParser(
			description='Remove a story from the Choosatron')

		parser.add_argument('index', action='story', type=int)
		args = parser.parse_args(sys.argv[2:])
		print 'Running git fetch, repository=%s' % args.repository

		data += CMD_START
		data += CMD_REMOVE_STORY
		data += chr(args.index)
		kCdamSerial.write(data)


if __name__ == '__main__':
	CDAMSerial()
