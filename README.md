Lemon Toolkit
=============
Author: Wong Cho Ching; License: BSD 2-clause license

Lemon Toolkit is a collection of reusable classes that I have developed. The classes are usually:

 * Written for fun
 * Extracted from an existing project

Currently, Lemon Toolkit contains the following classes:

 * RMap:		Recursive datatype with std::ostream<<serializer; and std::istream>>deserializer; and simple["access"]["method"]
 * NoCheat:		A cheat-proof datatype
 * NoOverflow:	A datatype that throws an exception when overflow/underflow error occurs

Please notice that C++11 support is required for using this library.

To use this toolkit, just copy this folder to your project.
Usually, you are only interested in one or two classes in this toolkit. In this case, you may remove all unused classes before adding the folder to your project.

The Makefile is used to build main.cpp, which runs test cases.

New classes may be added in the future.
Lemon Toolkit is NOT intended to be a comprehensive.
This is published for the purpose of saving other's time in case they encounter some specific problems.

