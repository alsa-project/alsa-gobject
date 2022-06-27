Title: Overview

This library is designed for applications to manipulate ALSA sequencer character device and operate
its functionality abstracted as user client, port, and event. ALSASeqUserClient express the user
client. It holds file descriptor and creates GSource for event dispatching by GLib's GMainContext
and GMainLoop.
