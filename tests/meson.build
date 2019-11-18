# Each entry includes:
#  key: the name of test script
#  value: namespace for metadata of gobject introspection
tests = {
  'ctl': [
    'alsactl-enums',
    'alsactl-card',
    'alsactl-card-info',
    'alsactl-elem-info',
    'alsactl-elem-info-bool',
    'alsactl-elem-info-int',
  ],
}

foreach path, scripts: tests
  objdir = join_paths(meson.build_root(), 'src', path)

  env = environment()
  env.append('LD_LIBRARY_PATH', objdir, separator : ':')
  env.append('GI_TYPELIB_PATH', objdir, separator : ':')

  foreach script: scripts
    prog = find_program(script)
    test(script, prog,
      env: env,
    )
  endforeach
endforeach
