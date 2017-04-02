QGPiano
=======

This is my attempt to use real ALICE data to compose music. Partially inspired by the chance compositions of John Cage and partially an excuse to procrastinate while writing my thesis, this analysis code takes in ALICE data (AOD format), and uses properties of the tracks in an event, for example the transverse momentum, or direction of travel in phi, eta, etc, to map these tracks onto a musical stave, outputting a text file in 'abc' notation.

This task is based on the ALICE tutorial by Redmer Bertens

I have used music21 to convert the abc text file into stave notation, which can be downloaded at <http://web.mit.edu/music21> . Music21 is a python-based toolkit for computer-aided musicology. Once downloaded, it is simple to parse the file containing the abc notation:

from music21 import *::
abcScore = converter.parse('myScore.abc')::


