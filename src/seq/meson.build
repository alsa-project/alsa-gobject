#
# Metadata part.
#

path = 'seq'
name = 'alsaseq'
namespace = 'ALSASeq'
has_enumerations = true
has_marshaller = false

sources = files(
  'query.c',
  'system-info.c',
  'client-info.c',
  'user-client.c',
  'addr.c',
  'port-info.c',
  'client-pool.c',
  'tstamp.c',
  'event-data-result.c',
  'event-data-note.c',
  'event-data-ctl.c',
  'event-data-queue.c',
  'event-data-connect.c',
  'subscribe-data.c',
  'queue-info.c',
  'queue-status.c',
  'queue-tempo.c',
  'queue-timer.c',
  'queue-timer-data-alsa.c',
  'remove-filter.c',
  'event-cntr.c',
)

headers = files(
  'alsaseq.h',
  'query.h',
  'system-info.h',
  'client-info.h',
  'user-client.h',
  'addr.h',
  'port-info.h',
  'client-pool.h',
  'tstamp.h',
  'event-data-result.h',
  'event-data-note.h',
  'event-data-ctl.h',
  'event-data-queue.h',
  'event-data-connect.h',
  'subscribe-data.h',
  'queue-info.h',
  'queue-status.h',
  'queue-tempo.h',
  'queue-timer.h',
  'queue-timer-data-alsa.h',
  'remove-filter.h',
  'event-cntr.h',
)

privates = files(
  'privates.h',
)

dependencies = [
  gobject_dependency,
  libudev_dependency,
  alsatimer_dependency,
]

# For test.
build_dirs += {'alsaseq': meson.current_build_dir()}

#
# Common part except for identifier of dependency.
#

inc_dir = join_paths(meson.project_name(), path)

# Generate enumerations for GObject fashion.
if has_enumerations
  enum_header = '@0@-enum-types.h'.format(name)
  headers += enum_header
  enums = gnome.mkenums_simple('@0@-enums'.format(name),
    sources: enum_header,
    symbol_prefix: name,
    identifier_prefix: namespace,
    install_header: true,
    install_dir: join_paths(get_option('includedir'), inc_dir),
    header_prefix: '#include <alsaseq.h>',
  )
else
  enums = []
endif

# Generate marshallers for GObject signals.
if has_marshaller
  marshallers = gnome.genmarshal('@0@-sigs-marshal'.format(name),
    prefix: '@0@_sigs_marshal'.format(name),
    sources: '@0@-sigs-marshal.list'.format(name),
    stdinc: true,
    install_header: true,
    install_dir: join_paths(get_option('includedir'), inc_dir),
  )
else
  marshallers = []
endif

# Constrain public APIs by linker option.
mapfile_name = '@0@.map'.format(name)
mapfile_path = join_paths(meson.current_source_dir(), mapfile_name)
linker_flag = '-Wl,--version-script,@0@'.format(mapfile_path)

library = library(name,
  sources: sources + headers + privates + enums + marshallers,
  version: meson.project_version(),
  soversion: meson.project_version().split('.')[0],
  install: true,
  dependencies: dependencies,
  link_args : linker_flag,
  link_depends : mapfile_name,
  include_directories: include_directories('.'),
)

install_headers(headers,
  subdir: inc_dir,
)

# Generate pkg-config file for development.
pkg.generate(library,
  description: 'GObject instrospection library for sequencer interface in asequencer.h',
  subdirs: inc_dir,
)

# Generate metadata for gobject introspection.
alsaseq_gir = gnome.generate_gir(library,
  sources: enums + marshallers + headers + sources,
  nsversion: '0.0',
  namespace: namespace,
  symbol_prefix: '@0@_'.format(name),
  identifier_prefix: namespace,
  export_packages: name,
  dependencies: dependencies,
  includes: [
    'GLib-2.0',
    'GObject-2.0',
    alsatimer_gir[0],
  ],
  header: 'alsaseq.h',
  install: true,
)
