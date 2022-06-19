import gi
gi.require_version('GObject', '2.0')
from gi.repository import GObject

def test_object(target, props, methods, signals) ->bool:
    labels = [prop.name for prop in target.props]
    for prop in props:
        if prop not in labels:
            print('Property {0} is not produced.'.format(prop))
            return False
    for method in methods:
        if not hasattr(target, method):
            print('Method {0} is not produced.'.format(method))
            return False
    labels = GObject.signal_list_names(target)
    for signal in signals:
        if signal not in labels:
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
