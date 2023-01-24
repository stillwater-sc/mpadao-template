# Mixed Precision Algorithm Development and Optimization

This repo contains a starting template for developing and optimizing mixed-precision algorithms for IoT, ML/DL/RL, SDR, and DSP applications that can be deployed on custom hardware accelerators while at the same time having an efficient collaboration with the host CPU.

Once you use this template, you will need to adjust the status badges below to point to your repo.

| **System** | **Status** | **More information** |
|------------|------------|----------------------|
| [FOSSA Status](https://app.fossa.com/projects/git%2Bgithub.com%2Fstillwater-sc%2Funiversal) | [![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fstillwater-sc%2Funiversal.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fstillwater-sc%2Funiversal?ref=badge_shield) | Open-source license dependency scanner Stillwater Universal|
| [GitHub Actions](https://github.com/stillwater-sc/mpadao-template/actions) | [![Build Status](https://github.com/stillwater-sc/mpadao-template/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/stillwater-sc/mpadao-template) | Latest Linux/MacOS/Windows builds and regression tests |


# How to build

This repo uses git submodules. The first step after pulling the repository is to configure the submodules:

```text
> git submodule init && git submodule update
```

After that, the repo is ready to be build:

```text
> mkdir build
> cd build
> cmake ..
> make
```

This will build the libraries, the CLI command projects, and the tests in `test/mpir_tests`.

When using VSCode, the repository contains a devcontainer spec in the directory $MPIR_ROOT/.devcontainer. 

![VS code environment](img/vscode-devcontainer.png)

The default container provides a build environment based on Clang14:

```json
{
	"image": "stillwater/universal:clang14builder"
}
```
There is a set of builders that can be used that contain specific compilers. Other environments are:
```text
gcc9builder
gcc10builder
gcc11builder
gcc12builder
clang11builder
clang12builder
clang13builder
clang14builder
```
If you want to change your development container, simply replace the json with the container of your choice.

You can also build natively. The .gitignore of this repo filters out the following directories:
```text
build/
build_msvc/
build_gcc/
build_clang/
```
You can use these build directories to organize your native and specific build containers so that they can run concurrently. For example, you can use the `build/` directory to hold native builds, and `build_gcc/` directory to hold the default build container builds.

# Install command line tools, libraries, and include files

To install the command line tools for ease of use, issue the `install` target:

```bash
> make install
```

This command will populate the $MPIR_ROOT/bin, $MPIR_ROOT/lib, and $MPIR_ROOT/include directories, where $MPIR_ROOT represents the directory path of the mixed-precision-ir repository clone.

If you are on a Linux or MacOS system, you can add the bin directory to your path to pick up the command line tools:

```bash
> export PATH=$PATH:$MPIR_ROOT/bin
```

For Windows, use the environment variable editor to do the same.


## Streamlining the Build

To just build the projects in mixed-precision-ir and ignore build targets in Universal and MTL4, use:

```zsh
> cmake -DBUILD_DEMONSTRATION=OFF -DENABLE_TESTS=OFF ..
```

## Updating the submodules

If you want to update the submodules to the latest version of the upstream repos, issue this command:

```bash
> git submodule update --remote --merge
```


# Project structure

The following figure shows the project structure of this repository:

![Project Structure](img/project-structure.png)
