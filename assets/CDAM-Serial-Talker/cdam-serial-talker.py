#!/usr/bin/env python

import subprocess
import argparse
import serial
import struct
import time
import glob
import sys
import os

import binascii

from modem import YMODEM

kCdamSerial = None

RESULT_SUCCESS = chr(0x01)
RESULT_FAILURE = chr(0x02)

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
   addstory     Add a story to the Choosatron
   removestory  Remove a story from the Choosatron
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
			kCdamSerial = serial.Serial(serialPaths[0], 9600, timeout=5)
			kCdamSerial.flushOutput()
			kCdamSerial.flushInput()
			# use dispatch pattern to invoke method with same name
			getattr(self, args.command)()
		else:
			print "No USB Serial Device Found"

	def getc(self, size, timeout=1):
		global kCdamSerial
		#data = kCdamSerial.read(size) or None
		#print "GET: " + ''.join('{:02x}'.format(x) for x in data)
		#print "GET: " + binascii.hexlify(data);
		data = kCdamSerial.read(size) or None
		kCdamSerial.flushInput()
		return data

	def putc(self, data, timeout=1):
		global kCdamSerial
		#print "PUT: " + binascii.hexlify(data)
		kCdamSerial.flushOutput()
		data = kCdamSerial.write(data) or None
		#kCdamSerial.flushOutput()
		#kCdamSerial.flush()
		return data

	def addstory(self):
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

		#serialPath = subprocess.check_output('ls /dev/cu.usb*', shell=True);
		#print serialPath

		data = bytearray()
		data += CMD_START
		data += CMD_ADD_STORY
		data += sizeBytes
		data += chr(args.index)
		time.sleep(1)
		kCdamSerial.write(data)
		response = kCdamSerial.read(77)
		print response
		kCdamSerial.flushOutput()
		kCdamSerial.flushInput()
		#try:
		ymodem = YMODEM(self.getc, self.putc)
		time.sleep(1)
		status = ymodem.send([args.filename])
		if status:
			print "Success!!!"
		else:
			print "Failed :("
		#except:
			#print "Unexpected error:", sys.exc_info()[0]
			#kCdamSerial.close()


	def removestory(self):
		parser = argparse.ArgumentParser(
			description='Remove a story from the Choosatron')

		parser.add_argument('index', action='store', type=int)
		args = parser.parse_args(sys.argv[2:])
		print 'Index=%d' % args.index

		data = bytearray()
		data += CMD_START
		data += CMD_REMOVE_STORY
		data += chr(args.index)
		kCdamSerial.write(data)
		result = kCdamSerial.read(1)

		if result == RESULT_SUCCESS:
			print "Success"
			exit(0)
		elif result == RESULT_FAILURE:
			print "Fail"
			exit(1)


if __name__ == '__main__':
	CDAMSerial()
