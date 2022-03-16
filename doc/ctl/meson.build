ext_contents = [
  'overview.md',
]

doc_toml = configure_file(
  input: 'alsactl.toml.in',
  output: 'alsactl.toml',
  configuration: conf,
)

custom_target('alsactl-doc',
  input: [ doc_toml, alsactl_gir[0] ],
  output: 'alsactl',
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
