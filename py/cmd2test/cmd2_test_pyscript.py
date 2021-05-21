
import os
import sys

result = app('dirlist {}'.format('.'))

print(self.teststr)


print(type(self))

self.do_hello()

self.teststr = "blah"
# Conditionally do something based on the results of the last command
if result:
    print(f"STDOUT: {result.stdout}\n")
    print(f"STDERR: {result.stderr}\n")
    print(f'DATA: {result.data}\n')

result = app('getmystr')
if result:
    print(f"STDOUT: {result.stdout}\n")
    print(f"STDERR: {result.stderr}\n")
    print(f'DATA: {result.data}\n')