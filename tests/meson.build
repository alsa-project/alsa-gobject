# Each entry includes:
#  key: the name of test script
#  value: namespace for metadata of gobject introspection
tests = {
  'ctl': [
    'alsactl-enums',
    'alsactl-card',
    'alsactl-card-info',
    'alsactl-elem-info-iec60958',
    'alsactl-elem-info-boolean',
    'alsactl-elem-info-bytes',
    'alsactl-elem-info-integer',
    'alsactl-elem-info-integer64',
    'alsactl-elem-info-enumerated',
    'alsactl-elem-value',
    'alsactl-elem-id',
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
    'alsatimer-device-id',
    'alsatimer-tick-event',
    'alsatimer-tstamp-event',
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
    'alsaseq-queue-timer-alsa',
    'alsaseq-addr',
    'alsaseq-event-cntr',
    'alsaseq-event',
    'alsaseq-event-data-connect',
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
foreach name, build_dir: build_dirs
  dir_paths += join_paths(build_dir)
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
