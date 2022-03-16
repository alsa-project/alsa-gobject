ext_contents = [
  'overview.md',
]

doc_toml = configure_file(
  input: 'alsaseq.toml.in',
  output: 'alsaseq.toml',
  configuration: conf,
)

custom_target('alsaseq-doc',
  input: [ doc_toml, alsaseq_gir[0] ],
  output: 'alsaseq',
  command: [
    gidocgen,
    'generate',
    '--no-namespace-dir',
    '--config=@INPUT0@',
    '--output-dir=@OUTPUT@',
    '--content-dir=@0@'.format(meson.current_source_dir()),
    '--add-include-path=@0@'.format(build_dirs['alsatimer']),
    '@INPUT1@',
  ],
  depend_files: [ ext_contents ],
  build_by_default: true,
  install: true,
  install_dir: doc_dir,
)
