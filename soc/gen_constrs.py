#!/usr/bin/env python3
from sys import argv

file = open(argv[1])
table = file.readlines()
for line in table:
	sections = line.split()
	print("""set_property PACKAGE_PIN %s [get_ports {%s}]		
	set_property IOSTANDARD LVCMOS33 [get_ports {%s}]""" %(sections[0], sections[1], sections[1]))
