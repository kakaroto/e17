build_ext:
	python setup.py build_ext

install: build_ext
	python setup.py install

uninstall:
	rm -rf /usr/lib/python/site-packages/python_elementary-0.1-py2.5-linux-i686.egg

clean:
	rm -rf build dist python_elementary-0.1-py2.5-linux-i686.egg python_elementary.egg-info 
	rm elementary/elementary.c_elementary.c
	rm elementary/*~
	rm include/*~

