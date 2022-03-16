conf = configuration_data()
conf.set('VERSION', meson.project_version())

doc_dir =  join_paths(get_option('datadir'), 'doc', meson.project_name())

doc_index = configure_file(
  input: 'index.html.in',
  output: 'index.html',
  configuration: conf,
)
install_data(doc_index,
  install_dir: doc_dir,
)

dependency('gi-docgen',
  version: '>= 2021.8',
  fallback: ['gi-docgen', 'dummy_dep'],
)
gidocgen = find_program('gi-docgen')

subdir('ctl')
subdir('hwdep')
subdir('rawmidi')
subdir('timer')
subdir('seq')
