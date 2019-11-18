#
# Metadata part.
#

path = 'ctl'
name = 'alsactl'
namespace = 'ALSACtl'
has_enumerations = true
has_marshaller = false

sources = files()

headers = files()

privates = files()

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

# Generate enumerations for GObject fashion.
if has_enumerations
  enum_header = '@0@-enum-types.h'.format(name)
  enums = gnome.mkenums_simple('@0@-enums'.format(name),
    sources: enum_header,
    symbol_prefix: name,
    identifier_prefix: namespace,
    install_header: true,
    install_dir: join_paths(get_option('includedir'), meson.project_name(), path),
    header_prefix: '#include <@0@-enum-types.h>'.format(join_paths(path, name)),
  )
  sources += enums
  headers += enum_header
endif

# Generate marshallers for GObject signals.
if has_marshaller
  signal_marshallers = gnome.genmarshal('@0@-sigs-marshal'.format(name),
    prefix: '@0@_sigs_marshal'.format(name),
    sources: '@0@-sigs-marshal.list'.format(name),
    stdinc: true,
    install_header: true,
  )
  sources += signal_marshallers
endif

# Constrain public APIs by linker option.
mapfile_name = '@0@.map'.format(name)
mapfile_path = join_paths(meson.current_source_dir(), mapfile_name)
linker_flag = '-Wl,--version-script,@0@'.format(mapfile_path)

library = library(name,
  sources: sources + headers + privates,
  version: meson.project_version(),
  soversion: meson.project_version().split('.')[0],
  install: true,
  dependencies: dependencies,
  link_args : linker_flag,
  link_depends : mapfile_name,
  include_directories: include_directories('..'),
)

install_headers(headers,
  subdir: join_paths(meson.project_name(), path),
)

# Generate pkg-config file for development.
pkg = import('pkgconfig')
pkg.generate(library,
  name: name,
  description: 'GObject instrospection library with API for asound.h and asequencer.h',
  subdirs: meson.project_name(),
)

# Generate metadata for gobject introspection.
gnome.generate_gir(library,
  sources: sources + headers,
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
  install: true,
)

# For documentation of gtk-doc.
alsactl_dir = include_directories('.')
alsactl_dependency = declare_dependency(
  link_with: library,
  dependencies: dependencies,
)
