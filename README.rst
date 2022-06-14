========================
The alsa-gobject project
========================

2022/06/06
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

`<https://alsa-project.github.io/alsa-gobject-docs/>`_

Python 3 Samples
================

See samples directory.

License
=======

This software is licensed under GNU Lesser General Public License version 3 or later

Dependencies
============

* GLib `<https://gitlab.gnome.org/GNOME/glib>`_
* GObject introspection `<https://gi.readthedocs.io/>`_
* libudev1 `<https://www.freedesktop.org/wiki/Software/systemd/>`_
* Linux kernel version 4.5 or later

Requirements to build
=====================

* UAPI header of Linux kernel
* Meson build system `<https://mesonbuild.com/>`_
* Ninja build system `<https://ninja-build.org/>`_
* PyGObject `<https://pygobject.readthedocs.io/>`_ (optional to run unit tests)
* gi-docgen `<https://gnome.pages.gitlab.gnome.org/gi-docgen/>`_ (optional to generate API documentation)

How to build
============

Preparation ::

    $ meson (--prefix=install-directory) build-directory

Build ::

    $ meson compile -C build-directory

Test ::

    $ meson test -C build-directory
    (PyGObject <https://pygobject.readthedocs.io/> is required)

Install ::

    $ meson install -C build-directory

After installed, C headers for APIs are available and pkg-config returns
arguments for them. For example ::

    $ pkg-config --cflags --libs alsactl
    -I/usr/include/alsa-gobject/ctl -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lalsactl

Generate documentation ::

    $ meson --prefix=install-directory -D doc=true build
    $ meson install -C build
    $ xdg-open (install-directory)/share/doc/alsa-gobject/index.html

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

* PyGObject `<https://pygobject.readthedocs.io/>`_ is a dynamic loader for
  libraries compatible with g-i.
* alsa-gobject-rs `<https://github.com/alsa-project/alsa-gobject-rs/>`_ includes
  creates to use these libraries.

Valgrind suppression file for leak detected in glib
===================================================

The execution of valgrind for executable binaries which link the libraries
detects some leaks inner glib, mainly due to allocation of type information.
This is expected and unavoidable. The `valgrind-glib.suppressions` file is
included in this repository and available to suppress the report.

For example ::

    $ valgrind --suppressions=valgrind-glib.suppressions --leak-check=full your-executable
