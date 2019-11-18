import gi
gi.require_version('GObject', '2.0')
from gi.repository import GObject

def test(target, props, methods, signals) ->bool:
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
