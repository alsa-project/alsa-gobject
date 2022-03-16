#
# Metadata part.
#

path = 'ctl'
name = 'alsactl'
namespace = 'ALSACtl'
has_enumerations = true
has_marshaller = true

sources = files(
  'query.c',
  'card.c',
  'card-info.c',
  'elem-id.c',
  'elem-info.c',
  'elem-value.c',
)

headers = files(
  'alsactl.h',
  'query.h',
  'card.h',
  'card-info.h',
  'elem-id.h',
  'elem-info.h',
  'elem-value.h',
)

privates = files(
  'privates.h',
)

# For test.
build_dirs += {'alsactl': meson.current_build_dir()}

#
# Common part except for identifier of dependency.
#

# Depends on glib-2.0 and gobject-2.0
gobject = dependency('gobject-2.0',
  version: '>=2.34.0'
)

libudev = dependency('libudev')

dependencies = [
  gobject,
  libudev,
]

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
    header_prefix: '#include <alsactl.h>',
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
pkg = import('pkgconfig')
pkg.generate(library,
  description: 'GObject instrospection library for control interface in asound.h',
  subdirs: inc_dir,
)

# Generate metadata for gobject introspection.
alsactl_gir = gnome.generate_gir(library,
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
  ],
  header: 'alsactl.h',
  install: true,
)
