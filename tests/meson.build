# Each entry includes:
#  key: the name of test script
#  value: namespace for metadata of gobject introspection
tests = {
  'ctl': [
    'alsactl-enums',
    'alsactl-card',
    'alsactl-card-info',
    'alsactl-elem-info',
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
    'alsatimer-instance-status',
  ],
  'seq': [
    'alsaseq-enums',
    'alsaseq-system-info',
    'alsaseq-client-info',
    'alsaseq-user-client',
    'alsaseq-port-info',
    'alsaseq-client-pool',
    'alsaseq-subscribe-data',
    'alsaseq-queue-info',
    'alsaseq-queue-status',
    'alsaseq-queue-tempo',
    'alsaseq-queue-timer',
    'alsaseq-event-cntr',
  ],
  'hwdep': [
    'alsahwdep-enums',
    'alsahwdep-device-info',
  ],
  'rawmidi': [
    'alsarawmidi-enums',
    'alsarawmidi-substream-info',
    'alsarawmidi-stream-pair',
    'alsarawmidi-substream-params',
    'alsarawmidi-substream-status',
  ],
}

dir_paths = []
foreach path, scripts: tests
  dir_paths += join_paths(meson.build_root(), 'src', path)
endforeach

env = environment()
env.append('LD_LIBRARY_PATH', ':'.join(dir_paths), separator : ':')
env.append('GI_TYPELIB_PATH', ':'.join(dir_paths), separator : ':')

foreach path, scripts: tests
  foreach script: scripts
    prog = find_program(script)
    test(script, prog,
      env: env,
    )
  endforeach
endforeach
