from music21 import *
import os

directory_in_str='/Users/norman/SoundParticle/First/Compositions_pp/'
directory = os.fsencode(directory_in_str)

for file in os.listdir(directory):
    filename = os.fsdecode(file)
    if filename.endswith(".abc"):
        c = converter.parse(directory_in_str+filename)
        midfilename=filename.replace('.abc','.mid')
        c.write('midi',directory_in_str+midfilename)
        print('converted %s to midi file %s' % (filename, midfilename))
    else:
        continue
