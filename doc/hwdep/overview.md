Title: Overview

This library is designed for applications to manipulate ALSA hwdep character device and retrieve
common information of hardware dependent functionality abstracted as hwdep device. As the design
concept means; hardware dependent, this library has no further functionality. Each applications
should implement it by their own. file descriptor and creates GSource for event dispatching by
GLib's GMainContext/GMainLoop.
