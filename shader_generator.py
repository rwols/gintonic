from sys import argv
import re

script, filename = argv

lines = [line.rstrip('\n') for line in open(filename)]

def handle_uniform_struct(lines, i):
	j = i
	print 'found uniform struct: ', lines[j]
	# continue on until we find a semicolon
	match = re.search('}', lines[j])
	while not match:
		j += 1
		match = re.search('}(.*?);', lines[j])
	structname = match.groups()[0].strip()
	print 'struct name:', structname

def handle_uniform(lines, i):
	print 'found uniform: ', lines[i]


i = 0
while i < len(lines):
	line = lines[i]
	match = re.search('\\s*uniform\\s+struct\\s+(\\w.*)', line)
	if match:
		structname = match.groups()[0].strip();
		print 'STRUCT: ', structname
		j = i + 1
		done_with_struct = False
		while not done_with_struct:
			
	else:
		match = re.search('\\s*uniform\\s+\\w.*\\s+(\\w.*);', line)
		if match:
			name = match.groups()[0].strip()
			print name
	i += 1