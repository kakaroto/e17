import eupnp


def mycb(evt_data, *args, **kwargs):
    print 'mycb: ', evt_data, args, kwargs
    return False

def mycb2(evt_data, *args, **kwargs):
    print 'mycb2: ', evt_data, args, kwargs
    return True

if __name__ == '__main__':
    mytype = eupnp.bus.event_type_new()
    eupnp.bus.subscribe(mytype, mycb, 1, 2, 3, a=5, b=6)
    h = eupnp.bus.subscribe(5, mycb2, 5, 6, c=3)

    eupnp.bus.event_publish(mytype, "Test!")
    eupnp.bus.event_publish(5, "Test1")

    eupnp.bus.unsubscribe(h)

    eupnp.shutdown()
