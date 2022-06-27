Title: Overview

This library is designed for applications to manipulate ALSA Rawmidi character device and operate
rawmidi functionality abstracted as a pair of streams and substream. ALSARawmidiStreamPair
expresss the pair of streams. It holds file descriptor and creates GSource for event dispathing
by GLib's GMainContext/GMainLoop.
