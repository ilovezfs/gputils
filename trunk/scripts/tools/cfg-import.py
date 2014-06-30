#!/usr/bin/env python

# Based on script from http://wayreth.rten.net/~ballbach/gputils/

"""This script reads 8bit_device.info from MPASM for configuration options
and output in standard output new libgputils/gpcfg-table.c."""

import sys
import copy
import struct
import pprint

############
### main ###
############

def split_info_line(line):
	line = line.rstrip("#\r\n")
	# Could check for first < and last >
	return line[1:-1].split("><")

def read_fields(f):
	line = f.readline()
	while line:
		fields = split_info_line(line)
		yield fields[0], fields[1:]
		line = f.readline()

def int16(s):
	return int(s, 16)

class reader(object):
	def __init__(self, f):
		self.r = read_fields(f)
		# Pic name to tuple of addresses
		self.pics = dict()
		# Addresses tuple to symbol name
		self.addrs = dict()
		# Keys tuple to symbol name
		self.keys = dict()
		# Values tuple to symbol name and option
		self.values = dict()

	def read_parts(self):
		while True:
			try:
				tag, args = self.r.next()
			except StopIteration:
				return
			if tag == 'PART_INFO_TYPE':
				(magichex, name, family, a,
				 pages, maxrom, banks, b,
				 c, d, e, numaddrs) = args
				numaddrs = int16(numaddrs)
				if numaddrs:
					addrs = self.read_addrs(name, numaddrs)
					if addrs not in self.addrs:
						self.addrs[addrs] = "gp_cfg_{0}_addrs".format(name.lower())
					self.pics[name] = tuple(addrs)

	def read_addrs(self, name, numaddrs):
		addrs = list()
		while numaddrs:
			tag, args = self.r.next()
			if tag == 'CONFIGREG_INFO_TYPE':
				addr, a, default, numkeys = map(int16, args)
				keys = self.read_keys(name, numkeys)
				if keys not in self.keys:
					self.keys[keys] = "{0}_{1:06x}".format(name.lower(), addr)
				addrs.append((addr, default, keys))
				numaddrs -= 1
		return tuple(addrs)

	def read_keys(self, name, numkeys):
		keys = list()
		while numkeys:
			tag, args = self.r.next()
			if tag == 'SWITCH_INFO_TYPE':
				opt, description, mask, numvalues = args
				mask = int16(mask)
				numvalues = int16(numvalues)
				values = self.read_values(numvalues)
				if values not in self.values:
					self.values[values] = ("{0}_{1}_opts".format(name.lower(), opt.lower()), opt)
				keys.append((opt, mask, values))
				numkeys -= 1
		return tuple(keys)

	def read_values(self, numvalues):
		values = []
		for i in range(numvalues):
			tag, args = self.r.next()
			if tag != 'SETTING_VALUE_TYPE':
				raise ValueError("Unknown tag {0}".format(tag))
			value_name, description, numeric = args
			numeric = int16(numeric)
			values.append((value_name, numeric))
		return tuple(values)

def write_h_file(fh, data):
	"""Writes a header file with all the pic data in it."""

	# header:
	print >> fh, "/*\n * This code is automatically generated from the 8bit_device.info file.\n" \
	             " * This data is in the public domain. Use care in modifying.\n */\n"
	print >> fh, "/*\n" \
	    " * Since caches are enabled, directive and option structures may seem to include\n" \
	    " * references to other PICs. This is done by caching when two or more PICs contain\n" \
	    " * identical data.\n" \
	    " */\n"

	print >> fh, '#include "stdhdr.h"'
	print >> fh, '#include "gpcfg.h"\n'

	for values, (name, opt) in sorted(data.values.items(), key=lambda x: x[1]):
		print >> fh, "static const struct gp_cfg_option {0}[] =".format(name)
		print >> fh, "{{ /* for {0}: */".format(opt)
		for valuename, value in values:
			print >> fh, "\t{ %-12s 0x%02x }," % \
			    (("\"%s\",") % valuename, value)
		print >> fh, "};\n"

	for keys, name in sorted(data.keys.items(), key=lambda x: x[1]):
		if not keys:
			continue
		print >> fh, "static const struct gp_cfg_directive {0}[] =".format(name)
		print >> fh, "{"
		for opt, mask, values in keys:
			opt_symbol_name, opt2 = data.values[values]
			if len(values) > 0:
				print >> fh, "\t{ %-12s 0x%02x, %2d, %s }," % \
				    ('"{0}",'.format(opt), mask, len(values),
				     opt_symbol_name)
		print >> fh, "};\n"

	for addrs, name in sorted(data.addrs.items(), key=lambda x: x[1]):
		print >> fh, "static const struct gp_cfg_addr {0}[] =".format(name)
		print >> fh, "{"
		for addr, default, keys in addrs:
			if keys:
				symbol_name = data.keys[keys]
			else:
				symbol_name = "NULL"
			print >> fh, "\t{ 0x%06x, 0x%02x, %2d, %s }," % \
			    (addr, default, len(keys), symbol_name)
		print >> fh, "};\n"

	# glue up _ALL_ devices !
	print >> fh, "/*\n" \
	             " * Devices Table\n" \
				 " */\n"
	print >> fh, "const unsigned gp_cfg_device_count = %d;" % len(data.pics)
	print >> fh, "const struct gp_cfg_device gp_cfg_devices[] =\n" \
			     "{"
	for pic, addrs in sorted(data.pics.items()):
		print >> fh, "\t{ %-15s %2d, %s }," % \
		    ('"{0}",'.format(pic), len(addrs), data.addrs[addrs])
	print >> fh, "};\n"

def main():
	# need a filename
	if len(sys.argv) != 2:
		print >> sys.stderr, "You must pass the location of the 8bit_device.info"
		sys.exit(1)

	with open(sys.argv[1], 'r') as f:
		data = reader(f)
		data.read_parts()

	# okay... I'll write a header file with all this data in it.
	write_h_file(sys.stdout, data)

#	pp = pprint.PrettyPrinter(indent = 2)
#	pp.pprint(pics)
#	print


##################
### entrypoint ###
##################

if __name__ == '__main__':
	main()


