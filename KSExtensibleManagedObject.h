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

- (id)extensiblePropertyForKey:(NSString *)key;
- (NSDictionary *)extensibleProperties;

- (void)setExtensibleProperty:(id)value forKey:(NSString *)key; // value should be non-nil
- (void)removeExtensiblePropertyForKey:(NSString *)key;

// Called after undoing or redoing change to a key. Default implementation does nothing, but should call super first when overriding
- (void)awakeFromExtensiblePropertyUndoUpdateForKey:(NSString *)key;


#pragma mark KVC/KVO Integration

/*!
 @method usesExtensiblePropertiesForUndefinedKey:
 @abstract Template method to let extensible properties be directly accessible from Key-Value Coding
 @param key The key being queried
 @result Default implementation returns NO for all keys
 @discussion Whenever a method such as -valueForUndefinedKey: or -setValue:forUndefinedKey: is called, the receiver will call this method to define the behaviour. If you return YES, the value will be stored/retrieved in/from extensible properties, and appropriate KVO notifications posted. If not, super's implementation will be invoked (i.e. raise an NSUndefinedKeyException)
 */
- (BOOL)usesExtensiblePropertiesForUndefinedKey:(NSString *)key;


#pragma mark Core Data Integration

/*	Takes the standard -changedValues and -committedValuesForKeys: methods and adds support for
 *	KSExtensibleManagedObject.
 */
- (NSDictionary *)committedValuesForKeys:(NSArray *)keys includeExtensibleProperties:(BOOL)flag;
- (NSDictionary *)changedValuesIncludingExtensibleProperties:(BOOL)flag;


#pragma mark Extensible Property Storage

/*!	These two methods are called by KSExtensibleManagedObject when archiving or unarchiving
 *	the dictionary it uses in-memory. You can override them in a subclass to tweak the
 *	behaviour. e.g. To use an encoding method other than NSKeyedArchiver.
 */
+ (NSDictionary *)unarchiveExtensibleProperties:(NSData *)propertiesData;
+ (NSData *)archiveExtensibleProperties:(NSDictionary *)properties;

/*!
 @method extensiblePropertiesDataKey
 @abstract Template method
 @result The key under which to store the archived representation of the extensible properties. By default this is "extensiblePropertiesData"
 @discussion Override this method to store extensible properties using a different key.
 */
+ (NSString *)extensiblePropertiesDataKey;


#pragma mark KVO Debugging
// Implements my suggestion from http://www.mikeabdullah.net/managed-object-kvo.html
// Please DO NOT turn this on for release builds as Apple could easily break your app
+ (BOOL)logsObserversWhenTurningIntoFault;
+ (void)setLogsObserversWhenTurningIntoFault:(BOOL)flag;


@end
