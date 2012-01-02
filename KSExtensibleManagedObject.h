//
//  KSExtensibleManagedObject.h
//
//  Copyright (c) 2007-2012 Mike Abdullah and Karelia Software
//  All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL MIKE ABDULLAH OR KARELIA SOFTWARE BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
