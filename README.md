Features
========

`KSExtensibleManagedObject` extends `NSManagedObject` to support dictionary-like storage of non-modelled properties. This is particularly useful in situations where you want to extend your managed object model slightly, but can't or don't want to perform a migration of existing persistent stores.

Contact
=======

I'm Mike Abdullah, of [Karelia Software](http://karelia.com). [@mikeabdullah](http://twitter.com/mikeabdullah) on Twitter.

Questions about the code should be left as issues at https://github.com/karelia/KSExtensibleManagedObject or message me on Twitter.

Dependencies
============

* No library dependencies beyond Core Data
* Works back to OS X v10.5.
* **BUT**, support for KSExtensibleManagedObject **MUST** be baked into your managed object model **BEFORE** shipping; see Usage below for full details

License
=======

Copyright © 2007 Karelia Software

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Usage
=====

When designing your managed object model, for any entities that you think you might want extensible property support for in the future, add an extra `data` attribute. By default `KSExtensibleManagedObject` assumes this attribute is named `extensiblePropertiesData`, so you likely want to match that. But if not, override `+extensiblePropertiesDataKey` to supply your own name.

Note that so far, you haven't needed the `KSExtensibleManagedObject` class itself. When/if you *do* require its functionality:

1. Add `KSExtensibleManagedObject.h` and `KSExtensibleManagedObject.m` to your project. Ideally, make this repo a submodule, but hey, it's your codebase, do whatever you feel like.
2. Make `KSExtensibleManagedObject` the superclass of your custom managed object's class
3. Call `-setExtensibleProperty:forKey:` or `-removeExtensiblePropertyForKey:` to modify the extensible property storage. Note that values must adopt `NSCoding` to be stored
4. Use `-extensiblePropertyForKey:` and `extensibleProperties` to retrieve values

You can go further if desired and turn on KVC and KVO (Key Value Observing/Coding) support for non-modelled keys by overriding `-usesExtensiblePropertiesForUndefinedKey:` in your custom subclass.

See the `KSExtensibleManagedObject.h` for a few other possibly helpful advanced goodies.
