conf = configuration_data()
conf.set('VERSION', meson.project_version())

configure_file(
  input : 'version.xml.in',
  output : 'version.xml',
  configuration : conf,
)

subdir('ctl')
subdir('timer')
subdir('seq')
subdir('hwdep')
