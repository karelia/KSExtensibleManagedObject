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

Licensed under the BSD License <http://www.opensource.org/licenses/bsd-license>
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
