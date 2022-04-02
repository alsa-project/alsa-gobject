#
# Metadata part.
#

path = 'hwdep'
name = 'alsahwdep'
namespace = 'ALSAHwdep'
has_enumerations = true
has_marshaller = false

sources = files(
  'query.c',
  'device-info.c',
)

headers = files(
  'query.h',
  'device-info.h',
)

privates = files(
  'privates.h',
)

dependencies = [
  gobject_dependency,
  libudev_dependency,
  utils_dependencies,
]

pc_desc = 'GObject instrospection library for HwDep interface in asound.h'

gir_includes = common_gir_includes

# For test.
build_dirs += {'alsahwdep': meson.current_build_dir()}

#
# Common part except for identifier of dependency.
#

inc_dir = join_paths(meson.project_name(), path)

top_header = '@0@.h'.format(name)
headers += top_header

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
    header_prefix: '#include <@0@>'.format(top_header),
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
  description: pc_desc,
  subdirs: inc_dir,
)

# Generate metadata for gobject introspection.
alsahwdep_gir = gnome.generate_gir(library,
  sources: enums + marshallers + headers + sources,
  nsversion: '0.0',
  namespace: namespace,
  symbol_prefix: '@0@_'.format(name),
  identifier_prefix: namespace,
  export_packages: name,
  dependencies: dependencies,
  includes: gir_includes,
  header: top_header,
  install: true,
)
