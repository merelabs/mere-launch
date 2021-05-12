# mere-launch
`mere-launch` is a small utility program to launch desktop applications (.desktop). if the argument only contain the aplication filename (w/ .desktopn extension), program will search for the application in locations specified in *xdg* specification.

```
To launch xterm application, one can command as
$ mere-launch xterm.desktop
```

## required ##
`mere-launch` requires following libaries,
- [mere-xdg](https://github.com/merelabs/mere-xdg-lib)
- [mere-utils](https://github.com/merelabs/mere-utils-lib)

## build ##
```
$ git clone https://github.com/merelabs/mere-launch.git
$ qmake
$ make
$ make install
```
### support
If you find a bug, or have a feature suggestion, please [log an issue](https://github.com/merelabs/mere-utils-lib/issues). If you'd like to
contribute, please read [How to Contribute](CONTRIBUTING.md).

## license ##
All the code in this repository is licensed under a [BSD-2-Clause License](LICENSE).
