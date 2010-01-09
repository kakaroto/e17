import eupnp
import ecore


if __name__ == '__main__':
    cp = eupnp.ControlPoint()

    if cp.start():
        eupnp.bus.subscribe(eupnp.bus.EVENT_DEVICE_READY, on_device_ready)
        ecore.main_loop_begin()
    else:
        print 'Failed to start control point, possibly network down.'

    cp.stop()
    del cp
    ecore.shutdown()
    eupnp.shutdown()
