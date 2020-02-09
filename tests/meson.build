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
    'alsactl-elem-info-enum',
    'alsactl-elem-info-bytes',
    'alsactl-elem-info-iec60958',
    'alsactl-elem-info-int64',
    'alsactl-elem-value',
  ],
  'timer': [
    'alsatimer-enums',
    'alsatimer-device-info',
    'alsatimer-device-status',
    'alsatimer-device-params',
    'alsatimer-user-instance',
    'alsatimer-instance-info',
    'alsatimer-instance-params',
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
