Title: Overview

This library is designed for applications to manipulate ALSA control character device and operate
control functionality abstracted as card and element. ALSACtlCard express the card. It holds
file descriptor and creates GSource for event dispatching by GLib's GMainContext/GMainLoop.
