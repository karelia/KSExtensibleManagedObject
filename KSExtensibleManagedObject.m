//
//  KSExtensibleManagedObject.m
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


#import "KSExtensibleManagedObject.h"

#if !__has_feature(objc_arc)
#   define KSEXTENSIBLEMANAGEDOBJECT_AUTORELEASE(x) ([(x) autorelease])
#   define KSEXTENSIBLEMANAGEDOBJECT_RELEASE(x) ([(x) release])
#else
#   define KSEXTENSIBLEMANAGEDOBJECT_AUTORELEASE(x) (x)
#   define KSEXTENSIBLEMANAGEDOBJECT_RELEASE(x) (x)
#endif

@interface KSExtensibleManagedObject (Private)

- (NSMutableDictionary *)primitiveExtensibleProperties;

+ (NSSet *)modifiedKeysBetweenDictionary:(NSDictionary *)dict1 andDictionary:(NSDictionary *)dict2;
- (NSDictionary *)archivedExtensibleProperties;

@end


#pragma mark -


@implementation KSExtensibleManagedObject

static BOOL sLogObservers = NO;

#pragma mark Extensible Properties

- (id)extensiblePropertyForKey:(NSString *)key;
{
    return [[self primitiveExtensibleProperties] valueForKey:key];
}

- (NSDictionary *)extensibleProperties
{
	NSDictionary *result = KSEXTENSIBLEMANAGEDOBJECT_AUTORELEASE([[self primitiveExtensibleProperties] copy]);
	return result;
}

- (void)setExtensibleProperty:(id)value forKey:(NSString *)key;
{
    [[self primitiveExtensibleProperties] setObject:value forKey:key];
    
    // Archive the new properties. This has to be done every time so Core Data knows
    // that some kind of change was made.
    [self setValue:[[self class] archiveExtensibleProperties:[self primitiveExtensibleProperties]]
            forKey:[[self class] extensiblePropertiesDataKey]];
}

- (void)removeExtensiblePropertyForKey:(NSString *)key;
{
    [[self primitiveExtensibleProperties] removeObjectForKey:key];
    
    // Archive the new properties. This has to be done every time so Core Data knows
    // that some kind of change was made.
    [self setValue:[[self class] archiveExtensibleProperties:[self primitiveExtensibleProperties]]
            forKey:[[self class] extensiblePropertiesDataKey]];
}

- (void)awakeFromExtensiblePropertyUndoUpdateForKey:(NSString *)key; { }

- (NSMutableDictionary *)primitiveExtensibleProperties;
{
	// Fault in the properties on-demand
	if (!_extensibleProperties)
	{
        @try {
            _extensibleProperties = [[NSMutableDictionary alloc] initWithDictionary:self.archivedExtensibleProperties];
            NSAssert(_extensibleProperties, @"-initWithDictionary: let me down");
        }
        @catch (NSException *exception) {
            // Catch and handle the exception by resetting properties to be empty. But then rethrow
            // the exception since the reset is only desirable if client code catches the exception
            // itself and chooses to proceed.
            if ([exception.name isEqualToString:NSInconsistentArchiveException]) {
                NSLog(@"Resetting extensible properties after failure to unarchive: %@", self);
                _extensibleProperties = [[NSMutableDictionary alloc] init];
            }

            @throw exception;
        }
	}
	
	return _extensibleProperties;
}

#pragma mark Extensible Property Storage

/*	Fetches all custom values from the persistent store rather than the in-memory representation.
 */
- (NSDictionary *)archivedExtensibleProperties
{
	NSDictionary *result = nil;
	
	NSString *key = [[self class] extensiblePropertiesDataKey];
	if (key)
	{
		NSData *data = [self valueForKey:key];
		result = [[self class] unarchiveExtensibleProperties:data];
	}
	
	return result;
}

+ (NSDictionary *)unarchiveExtensibleProperties:(NSData *)propertiesData
{
	NSMutableDictionary *result = nil;
	
	if (propertiesData)
	{
		id unarchivedDictionary = [NSKeyedUnarchiver unarchiveObjectWithData:propertiesData];
		if ([unarchivedDictionary isKindOfClass:[NSMutableDictionary class]])
		{
			result = unarchivedDictionary;
		}
	}
	
	return result;
}

+ (NSData *)archiveExtensibleProperties:(NSDictionary *)properties;
{
	NSData *result = [NSKeyedArchiver archivedDataWithRootObject:properties];
	return result;
}

+ (NSString *)extensiblePropertiesDataKey
{
	return @"extensiblePropertiesData";
}

+ (NSSet *)modifiedKeysBetweenDictionary:(NSDictionary *)dict1 andDictionary:(NSDictionary *)dict2
{
	// It's easy if either dictionary is nil
	if (!dict1) return [NSSet setWithArray:[dict2 allKeys]];
	if (!dict2) return [NSSet setWithArray:[dict1 allKeys]];
	
	
	// Build the set containing all the keys that exist in either dictionary
	NSMutableSet *allKeys = [[NSMutableSet alloc] initWithArray:[dict1 allKeys]];
	[allKeys addObjectsFromArray:[dict2 allKeys]];
	
	
	// Then run through these building a list of keys which the two dictionaries have different values for
	NSMutableSet *result = [NSMutableSet set];
	
	for (NSString *aKey in allKeys)
	{
		if (![[dict1 valueForKey:aKey] isEqual:[dict2 valueForKey:aKey]])
        {
            [result addObject:aKey];
		}
	}
	
	// Tidy up
	KSEXTENSIBLEMANAGEDOBJECT_RELEASE(allKeys);
	
	return result;
}

#pragma mark KVC Integration

- (BOOL)usesExtensiblePropertiesForUndefinedKey:(NSString *)key; { return NO; }

/*	We catch all undefined keys and pull them from the extensible properties dictionary.
 */
- (id)valueForUndefinedKey:(NSString *)key
{
	if ([self usesExtensiblePropertiesForUndefinedKey:key])
    {
        id result = [self extensiblePropertyForKey:key];
        return result;
    }
    else
    {
        return [super valueForUndefinedKey:key];
    }
}

/*	Undefined keys are caught and A) stored in-memory B) archived persistently
 */
- (void)setValue:(id)value forUndefinedKey:(NSString *)key
{
	NSParameterAssert(key);
    
    if ([self usesExtensiblePropertiesForUndefinedKey:key])
    {
        [self willChangeValueForKey:key];
        if (value)
        {
            [self setExtensibleProperty:value forKey:key];
        }
        else
        {
            [self removeExtensiblePropertyForKey:key];
        }
        [self didChangeValueForKey:key];
    }
    else
    {
        return [super setValue:value forUndefinedKey:key];
    }
}

#pragma mark Core Data Integration

- (void)awakeFromSnapshotEvents:(NSSnapshotEventType)flags;
{
    [super awakeFromSnapshotEvents:flags];
    
    
    // Comparison of the old and new dictionaries in order to to send out approrpriate KVO notifications
    // We specifically access the ivar directly to avoid faulting it in.
    NSDictionary *replacementDictionary = [self archivedExtensibleProperties];
    NSSet *modifiedKeys =
    [KSExtensibleManagedObject modifiedKeysBetweenDictionary:_extensibleProperties
                                               andDictionary:replacementDictionary];
    
    
    // Change each of the modified keys in our in-memory dictionary
    for (NSString *aKey in modifiedKeys)
    {
        BOOL fireKVONotificiations = [self usesExtensiblePropertiesForUndefinedKey:aKey];
        
        if (fireKVONotificiations) [self willChangeValueForKey:aKey];
        [[self primitiveExtensibleProperties] setValue:[replacementDictionary valueForKey:aKey]
                                                forKey:aKey];
        [self awakeFromExtensiblePropertyUndoUpdateForKey:aKey];
        if (fireKVONotificiations) [self didChangeValueForKey:aKey];
    }
}

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_6

/*	Whenever a change to our dictionary data is made due to an undo or redo, match the changes to
 *	our in-memory dictionary. Only needs to be done on 10.5, as 10.6 offers a proper API.
 */
- (void)didChangeValueForKey:(NSString *)key
{
	if (![NSManagedObject instancesRespondToSelector:@selector(awakeFromSnapshotEvents:)])
    {
        if ([key isEqualToString:self.class.extensiblePropertiesDataKey])
        {
		    NSUndoManager *undoManager = self.managedObjectContext.undoManager;
            if (undoManager.isUndoing || undoManager.isRedoing)
            {
                [self awakeFromSnapshotEvents:NSSnapshotEventUndoUpdate];
            }
        }
	}
	
	
	// Finally go ahead and do the default behavior. This is required to balance the
	// earlier -willChangeValueForKey: that must have ocurred.
	[super didChangeValueForKey:key];
}

#endif

/*	Throw away our internal dictionary just like normal Core Data faulting behavior.
 */
- (void)didTurnIntoFault
{
	if (sLogObservers && [self isDeleted] && [self observationInfo])
    {
        NSLog(@"%@ has observers:\n%@", [self objectID], [self observationInfo]);
    }
    
    KSEXTENSIBLEMANAGEDOBJECT_RELEASE(_extensibleProperties);	_extensibleProperties = nil;
    
	[super didTurnIntoFault];
}

/*	Extend the default behaviour of these 2 methods to take into account extensible properties
 */
- (NSDictionary *)committedValuesForKeys:(NSArray *)keys includeExtensibleProperties:(BOOL)flag
{
	if (!flag) return [self committedValuesForKeys:keys];
	
	
	NSMutableDictionary *buffer = [NSMutableDictionary dictionary];
	
	
	// Pull out the committed values
	NSArray *committedStandardKeys = nil;
	if (keys) {
		committedStandardKeys = [keys arrayByAddingObject:[[self class] extensiblePropertiesDataKey]];
	}
	NSDictionary *committedStandardProperties = [self committedValuesForKeys:committedStandardKeys];
	
	
	// Add required extensible keys to the buffer
	NSData *extensiblePropertiesData = [committedStandardProperties valueForKey:[[self class] extensiblePropertiesDataKey]];
	if (extensiblePropertiesData && (id)extensiblePropertiesData != [NSNull null])
	{
		NSDictionary *extensibleProperties = [[self class] unarchiveExtensibleProperties:extensiblePropertiesData];
		
		NSDictionary *requestedExtensibleProperties = extensibleProperties;
		if (keys)
		{
			requestedExtensibleProperties = [extensibleProperties dictionaryWithValuesForKeys:keys];
		}
		
		[buffer addEntriesFromDictionary:requestedExtensibleProperties];
	}
	
	
	// Add in the standard properties
	[buffer addEntriesFromDictionary:committedStandardProperties];
	
	
	// Unless specifically requested, leave out the extensible properties data
	if (!keys || ![keys containsObject:[[self class] extensiblePropertiesDataKey]])
	{
		[buffer removeObjectForKey:[[self class] extensiblePropertiesDataKey]];
	}
	
	
	// Tidy up
	NSDictionary *result = KSEXTENSIBLEMANAGEDOBJECT_AUTORELEASE([buffer copy]);
	return result;
}

- (NSDictionary *)changedValuesIncludingExtensibleProperties:(BOOL)flag
{
	NSMutableDictionary *result = KSEXTENSIBLEMANAGEDOBJECT_AUTORELEASE([[self changedValues] mutableCopy]);
	
	
	// If interested in extensible properties, replace the archived data with unarchived version
	if (flag && [result objectForKey:[[self class] extensiblePropertiesDataKey]])
	{
		[result removeObjectForKey:[[self class] extensiblePropertiesDataKey]];
		[result addEntriesFromDictionary:[self extensibleProperties]];
	}
	
	
	return result;
}

#pragma mark KVO Debugging

+ (BOOL)logsObserversWhenTurningIntoFault
{
    return sLogObservers;
}

+ (void)setLogsObserversWhenTurningIntoFault:(BOOL)flag
{
    sLogObservers = flag;
}

@end
