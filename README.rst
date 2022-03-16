========================
The alsa-gobject project
========================

2022/03/16
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
libalsahwdep0
    For APIs to operate ALSA hwdep character device
libalsarawmidi0
    For APIs to operate ALSA rawmidi character device

Namespaces of g-i produced by the project
=========================================

ALSACtl-0.0
    For bindings to use libalsactl0
ALSATimer-0.0
    For bindings to use libalsatimer0
ALSASeq-0.0
    For bindings to use libalsaseq0
ALSAHwdep-0.0
    For bindings to use libalsahwdep0
ALSARawmidi-0.0
    For bindings to use libalsarawmidi0

Documentation
=============

<https://alsa-project.github.io/alsa-gobject-docs/>

Python 3 Samples
================

See samples directory.

License
=======

This software is licensed under GNU Lesser General Public License version 3 or later

Dependencies
============

* GLib <https://gitlab.gnome.org/GNOME/glib>
* GObject introspection <https://gi.readthedocs.io/>
* UAPI of Linux kernel version 4.5 or later for sound subsystem
* libudev1 <https://www.freedesktop.org/wiki/Software/systemd/>
* Meson <https://mesonbuild.com/>
* (optional) PyGObject <https://pygobject.readthedocs.io/> to execute tests
* (optional) gi-docgen <https://gnome.pages.gitlab.gnome.org/gi-docgen/> to generate documentation

How to build
============

Preparation ::

    $ meson (--prefix=xxx) . build
    $ cd build

Build ::

    $ ninja

Test ::

    $ meson test
    (PyGObject <https://pygobject.readthedocs.io/> is required)

Install ::

    $ meson install

After installed, C headers for APIs are available and pkg-config returns
arguments for them. For example ::

    $ pkg-config --cflags --libs alsactl
    -I/usr/include/alsa-gobject/ctl -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lalsactl

Generate documentation ::

    $ meson --prefix=xxx -D doc=true . build
    $ cd build
    $ meson install
    $ xdg-open xxx/share/doc/html/index.html

Design note
===========

* These libraries are independent of alsa-lib, to focus on interaction to
  kernel land directly by system calls without alsa-lib's configuration space
  and plugin framework.
* The way to enumerate any device is based on sysfs, programmed with libudev1.
* GObject object is used for structures in UAPI of Linux sound subsystem with
  reserved space.
* Boxed object is used for structures in UAPI of Linux sound subsystem without
  reserved space.
* The type which has variable size of storage in 32/64 bit ABI (e.g. long) is
  not used for properties of GObject-derived objects.

Supplemental information for language bindings
==============================================

* PyGObject <https://pygobject.readthedocs.io/> is a dynamic loader for
  libraries compatible with g-i.
* alsa-gobject-rs <https://github.com/alsa-project/alsa-gobject-rs/> includes
  creates to use these libraries.

Valgrind suppression file for leak detected in glib
===================================================

The execution of valgrind for executable binaries which link the libraries
detects some leaks inner glib, mainly due to allocation of type information.
This is expected and unavoidable. The `valgrind-glib.suppressions` file is
included in this repository and available to suppress the report.

For example ::

    $ valgrind --suppressions=valgrind-glib.suppressions --leak-check=full your-executable
