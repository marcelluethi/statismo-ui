# Overview

statismo-ui is an (experimental) C++ interface for [scalismo-ui](https://github.com/unibas-gravis/scalismo-ui).

It works in pair with [ui-service](https://github.com/marcelluethi/ui-service).

# Requirements

## Statismo

You will need a fresh install of [statismo](https://github.com/kenavolic/statismo).

## Thrift (RPC framework)

You can find documentation about cpp install on [apache website](https://thrift.apache.org/lib/cpp) or [github](https://github.com/apache/thrift/blob/master/lib/cpp/README.md) and documenation about debian install on [apache website](https://thrift.apache.org/docs/install/debian).

Here is the procedure to install it through vcpkg on Ubuntu:
~~~
> sudo apt-get install flex bison
# If you don't use vcpkg for other stuff
# the build dir can be statismo-ui build directory
> mkdir build && cd build
> git clone https://github.com/Microsoft/vcpkg.git
> cd vcpkg
> ./bootstrap-vcpkg.sh
> ./vcpkg integrate install
> ./vcpkg install thrift
~~~

# Build

> :warning: This project is experimental and has been
> tested on Linux Ubuntu only

> :information_source: The procedure relies on thrift being
> installed with vcpkg

* Configure project
~~~
> cd build 
> cmake .. -DTHRIFT_HOME=${path_to_build_dir}/vcpkg/installed/x64-linux -Dstatismo_DIR=${path_to_install_dir}/lib/cmake/statismo
~~~
* Build
~~~
> cmake --build .
~~~

# Run examples

> :information_source: Project must be configure with *BUILD_EXAMPLES=ON*

* Run [ui-service](https://github.com/marcelluethi/ui-service) first
* Run an example
~~~
> cd build
> ./examples/viz-client
~~~

# Develop your own client

You can develop your own client for test or demo purpose. A common use case would be to visualize the
different steps of a model fitting transform.

# Notes

> :warning: Any breaking modification to the file
> ui.thrift must be applied to [ui-service](https://github.com/marcelluethi/ui-service) too.
