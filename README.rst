========================
The alsa-gobject project
========================

2020/04/04
Takashi Sakamoto

Introduction
============

This project is a sub project in Advanced Linux Sound Architecture a.k.a ALSA,
to produce libraries compatible with GObject introspection (g-i) for language
bindings.

Libraries produced by the project
=================================

libalsactl0
    For APIs to operate ALSA control character device
libalsatimer0
    For APIs to operate ALSA timer character device
libalsaseq0
    For APIs to operate ALSA seq character device, depending on libalsatimer0

Namespaces of g-i produced by the project
=========================================

ALSACtl-0.0
    For bindings to use libalsactl0
ALSATimer-0.0
    For bindings to use libalsatimer0
ALSASeq-0.0
    For bindings to use libalsaseq0

License
=======

This software is licensed under GNU Lesser General Public License version 3 or later

Dependencies
============

* GLib <https://gitlab.gnome.org/GNOME/glib>
* GObject introspection <https://gi.readthedocs.io/>
* UAPI of Linux kernel for sound subsystem
* libudev1 <https://www.freedesktop.org/wiki/Software/systemd/>
* Meson <https://mesonbuild.com/>
* (optional) PyGObject <https://pygobject.readthedocs.io/> to execute tests
* (optional) gtk-doc <https://www.gtk.org/gtk-doc/> to generate documentation

How to build
============

Preparation ::

    $ meson (--prefix=xxx) . build
    $ cd build

Build ::

    $ meson build

Test ::

    $ meson test
    (PyGObject <https://pygobject.readthedocs.io/> is required)

Install ::

    $ meson install

After installed, C headers for APIs are available and pkg-config returns
arguments for them. For example ::

    $ pkg-config --cflags --libs alsactl
    -I/usr/include/alsa-gobject -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lalsactl

Generate documentation ::

    $ meson --prefix=xxx -D gtk_doc=true . build
    $ cd build
    $ meson install
    $ xdg-open xxx/share/gtk-doc/html/alsactl/index.html

Design note
===========

* These libraries are independent of alsa-lib, to focus on interaction to
  kernel land directly by system calls without alsa-lib's configuration space.
* The way to enumerate any device is based on sysfs, programmed with libudev1.

Supplemental information for language bindings
==============================================

* PyGObject <https://pygobject.readthedocs.io/> is a dynamic loader for
  libraries compatible with g-i.
* alsa-gobject-rs <not published yet> includes creates to use these
  libraries.

Valgrind suppression file for leak detected in glib
===================================================

The execution of valgrind for executable binaries which link the libraries
detects some leaks inner glib, mainly due to allocation of type information.
This is expected and unavoidable. The `valgrind-glib.suppressions` file is
included in this repository and available to suppress the report.

For example ::

    $ valgrind --suppressions=valgrind-glib.suppressions --leak-check=full your-executable
