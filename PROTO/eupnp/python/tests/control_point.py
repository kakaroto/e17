import eupnp


if __name__ == '__main__':
    cp = eupnp.ControlPoint()
    print dir(cp)
    cp.start()
    cp.discovery_send(5, 'hey!')
    cp.stop()
    eupnp.shutdown()
