//
//  KSExtensibleManagedObject.h
//
//  Created by Mike Abdullah
//  Copyright © 2007 Karelia Software
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
//	A special kind of managed object that allows you to use -valueForKey: and
//	-setValueForKey: using any key. If the object does not normally accept this
//	key, it is stored internally in a dictionary and then archived as data.


#import <CoreData/CoreData.h>


@interface KSExtensibleManagedObject : NSManagedObject
{
  @private
	NSMutableDictionary	*_extensibleProperties;
}


#pragma mark Extensible Properties

/**
 Retrieves a value from the extensible properties store.
 
 @param key The key to lookup. Must not be `nil`.
 @result The value corresponding to `key`. Might be `nil`.
 */
- (id)extensiblePropertyForKey:(NSString *)key __attribute((nonnull(1)));

/**
 Retrieves a copy of the extensible properties store.
 
 @result All the extensible properties currently stored.
 */
- (NSDictionary *)extensibleProperties;

/**
 Stores an extensible property.
 
 @param value The value to store. Should not be `nil`.
 @param key The key to store. Must not be `nil`.
 */
- (void)setExtensibleProperty:(id)value forKey:(NSString *)key __attribute((nonnull(1,2)));

/**
 Removes an extensible property.
 
 @param key The key to remove from the extensible properties store. Must not be `nil`.
 */
- (void)removeExtensiblePropertyForKey:(NSString *)key __attribute((nonnull(1)));

/**
 Invoked automatically when an extensible properties is reset due to an undo or redo state change.
 
 Default implementation does nothing, but should call `super` first when
 overriding for future compatibility.
 
 @param key The key for whose values has changed.
 */
- (void)awakeFromExtensiblePropertyUndoUpdateForKey:(NSString *)key __attribute((nonnull(1)));


#pragma mark KVC/KVO Integration

/**
 Whether a given extensible property should be made KVC and KVO compliant.
 
 Whenever a method such as `-valueForUndefinedKey:` or
 `-setValue:forUndefinedKey:` is called, the receiver will call this method to
 define the behaviour. If you return `YES`, the value will be stored/retrieved
 in/from extensible properties, and appropriate KVO notifications posted. If not,
 `super`'s implementation will be invoked (i.e. `NSUndefinedKeyException` is
 raised).
 
 @param key The key being queried.
 @result Default implementation returns `NO` for all keys.
 */
- (BOOL)usesExtensiblePropertiesForUndefinedKey:(NSString *)key __attribute((nonnull(1)));


#pragma mark Core Data Integration

/**
 An extension to `-[NSManagedObject committedValuesForKeys:]` that can include extensible properties too.
 
 @param keys The keys to retrieve. Pass `nil` to retrieve all keys/values.
 @param flag Whether to include extensible properties in the list of changed values.
 @result The values last committed to disk.
 */
- (NSDictionary *)committedValuesForKeys:(NSArray *)keys includeExtensibleProperties:(BOOL)flag;

/**
 An extension to `-[NSManagedObject changedValues]` that can include extensible properties too.
 
 @param flag Whether to include extensible properties in the list of changed values.
 @result The changed values.
 */
- (NSDictionary *)changedValuesIncludingExtensibleProperties:(BOOL)flag;


#pragma mark Extensible Property Storage

/**
 Unarchives extensible properties.
 
 Subclasses can override to perform their own custom unarchiving.
 
 @param propertiesData The serialized extensible properties.
 @result The deserialized extensible properties.
 */
+ (NSDictionary *)unarchiveExtensibleProperties:(NSData *)propertiesData;

/**
 Archives extensible properties.
 
 Subclasses can override to perform their own custom archiving.
 
 @param properties The extensible properties.
 @result Deserialized representation of the extensible properties.
 */
+ (NSData *)archiveExtensibleProperties:(NSDictionary *)properties;

/**
 Override this method to store extensible properties using a different key.
 
 @result The key under which to store the archived representation of the extensible properties. By default this is `extensiblePropertiesData`.
 */
+ (NSString *)extensiblePropertiesDataKey;


#pragma mark KVO Debugging
// Implements my suggestion from http://www.mikeabdullah.net/managed-object-kvo.html
// Please DO NOT turn this on for release builds as Apple could easily break your app
+ (BOOL)logsObserversWhenTurningIntoFault;
+ (void)setLogsObserversWhenTurningIntoFault:(BOOL)flag;


@end
