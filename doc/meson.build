conf = configuration_data()
conf.set('VERSION', meson.project_version())

doc_dir =  join_paths(get_option('datadir'), 'doc', meson.project_name())

dependency('gi-docgen',
  version: '>= 2021.8',
  fallback: ['gi-docgen', 'dummy_dep'],
)
gidocgen = find_program('gi-docgen')

subdir('ctl')
subdir('hwdep')
subdir('rawmidi')
subdir('reference')
