headers = [
  'utils.h',
]

sources = [
]

dependencies = [
]

static_library = static_library('utils',
  sources: sources + headers,
)

utils_dependencies = declare_dependency(
  dependencies: dependencies,
  include_directories: include_directories('.'),
  link_with: static_library,
)
