========================
The alsa-gobject project
========================

2022/06/25
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
    For bindings to use the ``libalsactl0``
ALSATimer-0.0
    For bindings to use the ``libalsatimer0``
ALSASeq-0.0
    For bindings to use the ``libalsaseq0``
ALSAHwdep-0.0
    For bindings to use the ``libalsahwdep0``
ALSARawmidi-0.0
    For bindings to use the ``libalsarawmidi0``

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
* GObject-drived object is used for structures in UAPI of Linux sound subsystem with
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

Loss of backward compatibility between v0.2/0.3 releases
========================================================

Following to GNOME convention for throw function
------------------------------------------------

In GNOME convention, the throw function to report error at GError argument should return gboolean
value to report the overall operation finished successfully or not. At v0.3 release, the most of
public API are rewritten according to it.

Using GObject Interface
-----------------------

GObject Interface is utilized for some cases to express structure with union.

- ``ALSACtl.ElemInfoCommon`` and ``ALSACtl.ElemInfoSingleArray`` for ``struct snd_ctl_elem_info``
- ``ALSASeq.QueueTimerCommon`` for ``struct snd_seq_queue_timer``

Therefore some GObject-derived objects implements the interfaces.

- ``ALSACtl.ElemInfoIec60958``
- ``ALSACtl.ElemInfoBoolean``
- ``ALSACtl.ElemInfoBytes``
- ``ALSACtl.ElemInfoInteger``
- ``ALSACtl.ElemInfoInteger64``
- ``ALSACtl.ElemInfoEnumerated``
- ``ALSASeq.QueueTimerAlsa``

Some boxed structures are obsoleted and removed.

- ``ALSACtl.ElemInfo``
- ``ALSACtl.QueueTimer``
- ``ALSACtl.QueueTimerDataAlsa``

GObject Interface is utlized to define common feature of hwdep device as well.
``ALSAHwdep.DeviceCommon`` interface is added for the purpose.

Event expression for ALSA Sequencer
-----------------------------------

``ALSASeq.Event`` boxed structure is newly added to each event, and ``ALSASeq.EventCntr`` is
simplified to include deserializer only. ``ALSASeq.EventError`` domain is newly added to express
event handling problem.

Name consistency in time stamp expression
-----------------------------------------

The word ``tstamp`` is renamed to ``real time`` in the most of functions, enumerations, object
names, and methods for name consistency.

- ``ALSATimer.TstampEvent`` boxed structure is renamed to ``ALSATimer.RealTimeEvent``
- ``ALSATimer.EventType`` enumeration is renamed to ``ALSATimer.RealTimeEventType``
- ``ALSATimer.EventDataType`` enumeration is renamed to ``ALSATimer.EventType``
- ``ALSATimer.InstanceStatus.get_tstamp()`` method is renamed to ``ALSATimer.InstanceStatus.get_time()``
- ``ALSATimer.get_tstamp_source()`` function is renamed to ``ALSATimer.get_real_time_clock_id()``

In time stamp, the counterpart of ``real time`` is ``tick``. ``ALSATimer.TickEvent`` boxed structure
is renamed to ``ALSATimer.TickTimeEvent``. ``ALSATimer.Event``, ``ALSATimer.EventDataTick`` and
``ALSATimer.EventDataTstamp`` are obsoleted and removed.  ``ALSATimer.UserInstance::handle-event``
is obsoleted as well. The alternatives are available to retrieve corresponding event.

- ``ALSATimer.UserInstance::handle-tick-time-event``
- ``ALSATimer.UserInstance::handle-real-time-event``

``ALSASeq.Tstamp`` is obsoleted and removed as well. The accessor methods to it are rewritten
to retrieve either tick time or real time.

- ``ALSASeq.Event.get_tick_time()``
- ``ALSASeq.Event.get_real_time()``
- ``ALSASeq.Event.get_tick_time_data()``
- ``ALSASeq.Event.get_real_time_data()``
- ``ALSASeq.EventDataQueue.get_tick_time_param()``
- ``ALSASeq.EventDataQueue.get_real_time_param()``
- ``ALSASeq.RemoveFilter.new_with_tick_time()``
- ``ALSASeq.RemoveFilter.new_with_real_time()``

The word ``tstamp`` still remains in the enumerations and properties to affects both ``tick``
and ``real time``.

- ``ALSASeq.EventTstampMode``
- ``ALSASeq.Event.get_tstamp_mode()``
- ``ALSASeq.PortInfo:tstamp-mode``
- ``ALSASeq.PortInfo:tstamp-overwrite``
- ``ALSASeq.SusbscribeData:has-tstamp``
- ``ALSASeq.SusbscribeData:tstamp-mode``

Some properties are added to ``ALSASeq.SubscribeData`` to obsolete ``ALSASeq.PortSubscribeFlag``.

- ``ALSASeq.SubscribeData:is-exclusive``
- ``ALSASeq.SubscribeData:has-tstamp``
- ``ALSASeq.SubscribeData:tstamp-mode``
- ``ALSASeq.SubscribeData:queue-id``

Rewrite setter method in ALSACtl.ElemValue
------------------------------------------

Below methods are rewritten to retrieve the pointer in internal storage instead of copying to given
buffer:

- ``ALSACtl.ElemValue.get_bool``
- ``ALSACtl.ElemValue.get_int``
- ``ALSACtl.ElemValue.get_enum``
- ``ALSACtl.ElemValue.get_bytes``
- ``ALSACtl.ElemValue.get_iec60958_user_data``
- ``ALSACtl.ElemValue.get_iec60958_channel_status``
- ``ALSACtl.ElemValue.get_int64``

Rewrite ALSASeq.RemoveFilter
----------------------------

ALSASeq.RemoveFilter is largely written so that it's GObject-derived object instead of boxed
structure for flexible configuration of filter condition.
