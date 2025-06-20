# Moved to Github

This repository was moved to Github. My original repository has ~7 years of activity from 2016-04-27 to 2023-01-03: https://bitbucket.org/nicaraguabinary/lib-auframework-src/
  
# lib-auframework-src

Library to build command-line-tools and cores for visual-apps and services in C++.

This library depends of:

- [sys-nbframework-src](https://github.com/marcosjom/sys-nbframework-src)


# Spanish Language

Created by [Marcos Ortega](https://mortegam.com/).

This library was designed in spanish, intended to be used by latin-american university students to build academic and professional projects.

[sys-nbframework-src](https://github.com/marcosjom/sys-nbframework-src) is a professionally-intended evolition of this library, in english.

# Deprecated

It is deprecated; its purpose and features are being moved to [sys-nbframework-src](https://github.com/marcosjom/sys-nbframework-src).

# How to compile

For simplicity, create this folder structure:

- my_folder
   - sys-auframework<br/>
      - [lib-auframework-src](https://github.com/marcosjom/lib-auframework-src)<br/>
   - sys-nbframework<br/>
      - [sys-nbframework-src](https://github.com/marcosjom/sys-nbframework-src)<br/>

You can create your own folders structure but it will require to update some paths in the projects and scripts.

The following steps will create static libraries. In your project you should link to each `static-libray` and add a reference to its `include` folder.

## Windows

Open `projects/visual-studio/auframework.sln` and compile the desired target.

## MacOS and iOS

Open `projects/xcode/lib-auframework.xcworkspace` and compile the desired target.

## Android

In a terminal:

```
cd lib-auframework-src
make NB_CFG_HOST=Android
```

Check each project's `Makefile` and `MakefileProject.mk` files, and the [MakefileFuncs.mk](https://github.com/marcosjom/sys-nbframework-src/blob/main/MakefileFuncs.mk) to understand the `make` process, including the accepted flags and targets. 

## Linux and Others

In a terminal:

```
cd lib-auframework-src
make
```

Check each project's `Makefile` and `MakefileProject.mk` files, and the [MakefileFuncs.mk](https://github.com/marcosjom/sys-nbframework-src/blob/main/MakefileFuncs.mk) to understand the `make` process, including the accepted flags and targets.

# Contact

Visit [mortegam.com](https://mortegam.com/) for more information and visual examples of projects built with this libray.

May you be surrounded by passionate and curious people. :-)
