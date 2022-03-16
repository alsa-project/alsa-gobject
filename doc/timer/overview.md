Title: Overview

This library is designed for applications to manipulate ALSA timer character device and operate
timer functionality abstracted as timer device and user instance. ALSATimerUserInstance represents
the user instance. It holds file descriptor and creates GSource for event dispatching by GLib's
GMainContext/GMainLoop.
