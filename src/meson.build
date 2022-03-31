# Depends on glib-2.0 and gobject-2.0
gobject_dependency = dependency('gobject-2.0',
  version: '>=2.34.0'
)

libudev_dependency = dependency('libudev')

gnome = import('gnome')
pkg = import('pkgconfig')

common_gir_includes = [
    'GLib-2.0',
    'GObject-2.0',
]

build_dirs = {}

subdir('ctl')
subdir('timer')
subdir('seq')
subdir('hwdep')
subdir('rawmidi')
