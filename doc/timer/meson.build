ext_contents = [
  'overview.md',
]

doc_toml = configure_file(
  input: 'alsatimer.toml.in',
  output: 'alsatimer.toml',
  configuration: conf,
)

custom_target('alsatimer-doc',
  input: [ doc_toml, alsatimer_gir[0] ],
  output: 'alsatimer',
  command: [
    gidocgen,
    'generate',
    '--no-namespace-dir',
    '--config=@INPUT0@',
    '--output-dir=@OUTPUT@',
    '--content-dir=@0@'.format(meson.current_source_dir()),
    '@INPUT1@',
  ],
  depend_files: [ ext_contents ],
  build_by_default: true,
  install: true,
  install_dir: doc_dir,
)
