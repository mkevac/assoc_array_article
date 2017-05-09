INSTALL
-------

```
$ git submodule update --init
$ make
$ ./bench.py | tee results
$ ./chart.py results
$ xdg-open charts.html
```

![screenshot](screenshot.png)
