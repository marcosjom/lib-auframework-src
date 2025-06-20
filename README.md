# Moved to Github

This repository was moved to Github. My original repository has ~7 years of activity from 2016-04-27 to 2023-01-03: https://bitbucket.org/nicaraguabinary/lib-auframework-src/
  
# lib-auframework-src

Library to build command-line-tools and cores for visual-apps and services in C++.

This library depends of:

- [sys-nbframework-src](https://github.com/marcosjom/sys-nbframework-src)


# Spanish Language

Created by Marcos Ortega.

This library was designed in spanish, intended to be used by latin-american university students to build academic and professional projects.

sys-nbframework-src is an english and professionally-intended evolition of this library. 

# Deprecated

It is deprecated; its fucntionalities are/were moved to lib-nbframework-src.

# How to compile

For simplicity, create this folder structure:

```
my_folder
   |- sys-auframework
      |- [lib-auframework-src](https://github.com/marcosjom/lib-auframework-src)
   |- sys-nbframework
      |- [sys-nbframework-src](https://github.com/marcosjom/sys-nbframework-src)
```

You can create your own folders structure but it will require to update some paths in the projects and scripts.

The following steps will create static libraries to be linked in your own project. You should add a reference the each 'include' folder in yoru project.

##Windows

Open `projects/visual-studio/auframework.sln`and compile target.

##MacOS and iOS

Open `projects/xcode/lib-auframework.xcworkspace`and compile target.

##Android

In a terminal:

```
cd lib-auframework-src
make NB_CFG_HOST=Android
```

Check the [MakefileFuncs.mk](https://github.com/marcosjom/sys-nbframework-src/blob/main/MakefileFuncs.mk) file, and the `Makefile` and `MakefileProject.mk` files of each project to understand the `make` process, includint the accepted flags. 

##Linux and Others

In a terminal:

```
cd lib-auframework-src
make
```
Check the [MakefileFuncs.mk](https://github.com/marcosjom/sys-nbframework-src/blob/main/MakefileFuncs.mk) file, and the `Makefile` and `MakefileProject.mk` files of each project to understand the `make` process, includint the accepted flags.

##Contact

Send me a message [me@mortegam.com](mailto:me@mortegam.com) or visit [mortegam.com](https://mortegam.com/) for more information.

May you be surrounded by passionate and curious people. :-)
