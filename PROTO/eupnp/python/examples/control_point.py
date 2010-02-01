import eupnp
import ecore


class ControlPoint(eupnp.ControlPoint):

    def __init__(self):
        eupnp.bus.subscribe(eupnp.bus.EVENT_DEVICE_READY, self.on_device_ready)

    def on_device_ready(self, device, *args, **kwargs):
        print 'Device ready:', device

        for v in device.services.values():
            print '\t', v

        return True


if __name__ == '__main__':
    c = ControlPoint()
    c.start()
    c.discovery_send(5, "upnp:rootdevice")
    ecore.main_loop_begin()
    c.stop()

    del c
    ecore.shutdown()
    eupnp.shutdown()
