headers = [
  'utils.h',
]

sources = [
  'string.c',
  'sysfs.c',
  'ioctl.c',
]

dependencies = [
  dependency('libudev'),
]

static_library = static_library('utils',
  sources: sources + headers,
)

utils_dependencies = declare_dependency(
  dependencies: dependencies,
  include_directories: include_directories('.'),
  link_with: static_library,
)
