def test_object(target_type: object, props: tuple[str], methods: tuple[str],
                vmethods: tuple[str], signals: tuple[str]) -> bool:
    # All of available methods are put into the list of attribute.
    for method in methods:
        if not hasattr(target_type, method):
            print('Method {0} is not produced.'.format(method))
            return False

    # The properties, virtual methods, and signals in interface are not put
    # into the list of attribute in object implementing the interface.
    prop_labels = []
    vmethod_labels = []
    signal_labels = []

    # The  gi.ObjectInfo and gi.InterfaceInfo keeps them. Let's traverse them.
    for info in target_type.__mro__:
        if hasattr(info, '__info__'):
            for prop in info.__info__.get_properties():
                prop_labels.append(prop.get_name())
            for vfunc in info.__info__.get_vfuncs():
                vmethod_labels.append('do_' + vfunc.get_name())
            for signal in info.__info__.get_signals():
                signal_labels.append(signal.get_name())

    for prop in props:
        if prop not in prop_labels:
            print('Property {0} is not produced.'.format(prop))
            return False
    for vmethod in vmethods:
        if vmethod not in vmethod_labels:
            print('Vmethod {0} is not produced.'.format(vmethod))
            return False
    for signal in signals:
        if signal not in signal_labels:
            print('Signal {0} is not produced.'.format(signal))
            return False
    return True


def test_enums(target_type: object, enumerations: tuple[str]) -> bool:
    for enumeration in enumerations:
        if not hasattr(target_type, enumeration):
            print('Enumeration {0} is not produced for {1}.'.format(
                enumeration, target_type))
            return False
    return True


def test_struct(target_type: object, methods: tuple[str]) -> bool:
    for method in methods:
        if not hasattr(target_type, method):
            print('Method {0} is not produced.'.format(method))
            return False
    return True
