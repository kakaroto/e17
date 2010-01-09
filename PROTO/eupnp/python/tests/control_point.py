import eupnp


if __name__ == '__main__':
    cp = eupnp.ControlPoint()
    cp.start()
    cp.discovery_send(5, 'upnp:rootdevice')
    cp.stop()
    del cp
    eupnp.shutdown()
