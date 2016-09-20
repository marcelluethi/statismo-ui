# Statismo UI

An (experimental) C++ interface for [scalismo-ui](github.com/unibas-gravis/scalismo-ui).


### Building statismo UI

* Go to the statismo-ui folder

* Create a build folder

``` mkdir build```

* Change the directory to this build folder
``` cd build```

* Run CMake (e.g. ccmake) from this build folder with the given source directory ``` ccmake .. ```

* Press configure and set the
    * thrift include directory
    * thrift library directory
    * statismo build or installation directory

* Press generate
* Leave CMake

* Generate the thrift header files by issueing ```thrift -gen cpp ui.thrift```


* Compile the project either from your IDE (on windows) or by calling ```make``` (on linux)

* Install the project by calling ```make install```
