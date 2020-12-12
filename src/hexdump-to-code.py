dump="""
0600: a5 00 69 01 4c 02 06 
"""
out = []
for line in dump.split('\n'):
  if not line:
    continue
  out += line[5::].split(' ')[1:-1:]

print('{0x' + ', 0x'.join(out) + '}')