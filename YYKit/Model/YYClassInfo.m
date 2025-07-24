//
//  YYClassInfo.m
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 15/5/9.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import "YYClassInfo.h"
#import <objc/runtime.h>

YYEncodingType YYEncodingGetType(const char *typeEncoding) {
    char *type = (char *)typeEncoding;
    // 如果为空，则返回未知类型
    if (!type) return YYEncodingTypeUnknown;
    size_t len = strlen(type);
    // 如果长度为0，返回未知类型
    if (len == 0) return YYEncodingTypeUnknown;
    
    /**
     通过前缀获取类型限定符，如果前缀字符不匹配则终止类型限定符的获取
     type++ 表示移动字符位置
     */
    YYEncodingType qualifier = 0;
    bool prefix = true;
    while (prefix) {
        switch (*type) {
            case 'r': {
                qualifier |= YYEncodingTypeQualifierConst;
                type++;
            } break;
            case 'n': {
                qualifier |= YYEncodingTypeQualifierIn;
                type++;
            } break;
            case 'N': {
                qualifier |= YYEncodingTypeQualifierInout;
                type++;
            } break;
            case 'o': {
                qualifier |= YYEncodingTypeQualifierOut;
                type++;
            } break;
            case 'O': {
                qualifier |= YYEncodingTypeQualifierBycopy;
                type++;
            } break;
            case 'R': {
                qualifier |= YYEncodingTypeQualifierByref;
                type++;
            } break;
            case 'V': {
                qualifier |= YYEncodingTypeQualifierOneway;
                type++;
            } break;
            default: { prefix = false; } break;
        }
    }

    len = strlen(type);
    if (len == 0) return YYEncodingTypeUnknown | qualifier;

    // 获取类型标识
    switch (*type) {
        case 'v': return YYEncodingTypeVoid | qualifier;
        case 'B': return YYEncodingTypeBool | qualifier;
        case 'c': return YYEncodingTypeInt8 | qualifier;
        case 'C': return YYEncodingTypeUInt8 | qualifier;
        case 's': return YYEncodingTypeInt16 | qualifier;
        case 'S': return YYEncodingTypeUInt16 | qualifier;
        case 'i': return YYEncodingTypeInt32 | qualifier;
        case 'I': return YYEncodingTypeUInt32 | qualifier;
        case 'l': return YYEncodingTypeInt32 | qualifier;
        case 'L': return YYEncodingTypeUInt32 | qualifier;
        case 'q': return YYEncodingTypeInt64 | qualifier;
        case 'Q': return YYEncodingTypeUInt64 | qualifier;
        case 'f': return YYEncodingTypeFloat | qualifier;
        case 'd': return YYEncodingTypeDouble | qualifier;
        case 'D': return YYEncodingTypeLongDouble | qualifier;
        case '#': return YYEncodingTypeClass | qualifier;
        case ':': return YYEncodingTypeSEL | qualifier;
        case '*': return YYEncodingTypeCString | qualifier;
        case '^': return YYEncodingTypePointer | qualifier;
        case '[': return YYEncodingTypeCArray | qualifier;
        case '(': return YYEncodingTypeUnion | qualifier;
        case '{': return YYEncodingTypeStruct | qualifier;
        case '@': { // `@`开头的可能是对象类型，也可能是`block`类型
            if (len == 2 && *(type + 1) == '?')
                return YYEncodingTypeBlock | qualifier;
            else
                return YYEncodingTypeObject | qualifier;
        }
        default: return YYEncodingTypeUnknown | qualifier;
    }
}

@implementation YYClassIvarInfo

- (instancetype)initWithIvar:(Ivar)ivar {
    if (!ivar) return nil;
    self = [super init];
    _ivar = ivar;
    // 通过`ivar`获取该成员变量名称
    const char *name = ivar_getName(ivar);
    if (name) {
        _name = [NSString stringWithUTF8String:name];
    }
    // 获取该成员变量的偏移量
    _offset = ivar_getOffset(ivar);
    // 获取成员变量的类型字符串
    const char *typeEncoding = ivar_getTypeEncoding(ivar);
    
    if (typeEncoding) {
        _typeEncoding = [NSString stringWithUTF8String:typeEncoding];
        _type = YYEncodingGetType(typeEncoding);
    }
    return self;
}

@end

@implementation YYClassMethodInfo

- (instancetype)initWithMethod:(Method)method {
    if (!method) return nil;
    self = [super init];
    // 原始方法
    _method = method;
    // 获取方法选择器（selector），是指向存放方法名称内存区域的指针，类型是`SEL`
    _sel = method_getName(method);
    // 获取方法实现，代表函数指针，指向方法实现的首地址（函数执行的入口）
    _imp = method_getImplementation(method);
    // 通过方法选择器获取C字符串的方法名
    const char *name = sel_getName(_sel);
    
    if (name) {
        _name = [NSString stringWithUTF8String:name];
    }
    
    /**
     获取方法编码
     `- (void)setStat:(NSString *)stat` ----> `v24@0:8@16`
        `v`: 代表方法返回值为void
        `24`: 方法参数占24个字节（8 + 8 + 8）
        `@`: 方法的第一个参数self
        `0`: 方法的第一个参数的self起始地址偏移量
        `:`: 方法的第二个参数_cmd
        `8`: 方法的第二个参数的_cmd起始地址偏移量
        `@`: 方法的第三个参数stat，任何对象类型的参数编码都是@
        `16`: 方法的第二个参数的_cmd起始地址偏移量
     `- (NSString *)getName:(int)type` ----> `@20@0:8i16`
         `@`: 代表方法返回值，任何对象类型的参数编码都是@
         `20`: 方法参数占24个字节（8 + 8 + 4）
         `@`: 方法的第一个参数self
         `0`: 方法的第一个参数的self起始地址偏移量
         `:`: 方法的第二个参数_cmd
         `8`: 方法的第二个参数的_cmd起始地址偏移量
         `i`: 方法的第三个参数type，int类型参数编码都是i
         `16`: 方法的第二个参数的_cmd起始地址偏移量
     */
    const char *typeEncoding = method_getTypeEncoding(method);
    
    if (typeEncoding) {
        _typeEncoding = [NSString stringWithUTF8String:typeEncoding];
    }
    
    // 获取方法的返回值类型编码
    char *returnType = method_copyReturnType(method);
    
    if (returnType) {
        _returnTypeEncoding = [NSString stringWithUTF8String:returnType];
        free(returnType);
    }
    // 获取方法的参数个数（包含隐式参数`self`和`_cmd`）
    unsigned int argumentCount = method_getNumberOfArguments(method);
    
    if (argumentCount > 0) {
        NSMutableArray *argumentTypes = [NSMutableArray new];
        for (unsigned int i = 0; i < argumentCount; i++) {
            char *argumentType = method_copyArgumentType(method, i);
            NSString *type = argumentType ? [NSString stringWithUTF8String:argumentType] : nil;
            NSLog(@"cb-argumentType:%@",type);

            [argumentTypes addObject:type ? type : @""];
            if (argumentType) free(argumentType);
        }
        
        _argumentTypeEncodings = argumentTypes;
    }
    
    return self;
}

@end

@implementation YYClassPropertyInfo

- (instancetype)initWithProperty:(objc_property_t)property {
    if (!property) return nil;
    self = [super init];
    // 保留原始属性结构体
    _property = property;
    // 获取属性名
    const char *name = property_getName(property);
    if (name) {
        _name = [NSString stringWithUTF8String:name];
    }
    
    YYEncodingType type = 0;
    unsigned int attrCount;
    /**
     获取属性详细信息
     `property_copyAttributeList` 会获取到属性详细信息
     包括属性的类型、原子性/非原子性、内存策略、以及该属性的ivar成员变量
     */
    
    objc_property_attribute_t *attrs = property_copyAttributeList(property, &attrCount);
    
    for (unsigned int i = 0; i < attrCount; i++) {
        
        switch (attrs[i].name[0]) {
            case 'T': { // 属性的类型
                if (attrs[i].value) {
                    _typeEncoding = [NSString stringWithUTF8String:attrs[i].value];
                    type = YYEncodingGetType(attrs[i].value);
                    
                    /**
                     通过按位与操作，可以提取复合类型中的0xFF部分数据，即从一个包含限定符、属性、类型的type中清除除类型外的其它信息，只获取类型信息
                        @code
                            YYEncodingType type = YYEncodingTypePropertyNonatomic | YYEncodingTypeQualifierConst | YYEncodingTypePropertyNonatomic
                            通过对应的掩码可以提取复合信息中的特定数据：
                                `(type & YYEncodingTypeMask)` 获取类型标识
                                `(type & YYEncodingTypeQualifierMask)` 获取限定符标识
                                `(type & YYEncodingTypePropertyMask)` 获取属性标识
                     */
                    // 如果是对象类型
                    if ((type & YYEncodingTypeMask) == YYEncodingTypeObject && _typeEncoding.length) {
                        NSScanner *scanner = [NSScanner scannerWithString:_typeEncoding];
                        
                        // 扫描字符串的开头部分，如果`_typeEncoding`开头能匹配到`@"`
                        // 则表明该属性是一个对象类型
                        if (![scanner scanString:@"@\"" intoString:NULL]) continue;
                        
                        
                        NSString *clsName = nil;
                        // 扫描找到#或者<部分停止
                        if ([scanner scanUpToCharactersFromSet: [NSCharacterSet characterSetWithCharactersInString:@"\"<"] intoString:&clsName]) {
                            if (clsName.length) _cls = objc_getClass(clsName.UTF8String);
                        }
                        
                        NSMutableArray *protocols = nil;
                        while ([scanner scanString:@"<" intoString:NULL]) {
                            NSString* protocol = nil;
                            if ([scanner scanUpToString:@">" intoString: &protocol]) {
                                if (protocol.length) {
                                    if (!protocols) protocols = [NSMutableArray new];
                                    [protocols addObject:protocol];
                                }
                            }
                            [scanner scanString:@">" intoString:NULL];
                        }
                        _protocols = protocols;
                    }
                }
            } break;
            case 'V': { // 属性对应的成员变量ivar
                if (attrs[i].value) {
                    _ivarName = [NSString stringWithUTF8String:attrs[i].value];
                }
            } break;
            case 'R': { // readonly
                type |= YYEncodingTypePropertyReadonly;
            } break;
            case 'C': { // copy
                type |= YYEncodingTypePropertyCopy;
            } break;
            case '&': { // 默认修饰符，比如readwrite
                type |= YYEncodingTypePropertyRetain;
            } break;
            case 'N': { // nonatomic 非原子性
                type |= YYEncodingTypePropertyNonatomic;
            } break;
            case 'D': { // dynamic
                type |= YYEncodingTypePropertyDynamic;
            } break;
            case 'W': { // weak
                type |= YYEncodingTypePropertyWeak;
            } break;
            case 'G': { // getter 自定义属性的Getter方法
                type |= YYEncodingTypePropertyCustomGetter;
                if (attrs[i].value) {
                    
                    _getter = NSSelectorFromString([NSString stringWithUTF8String:attrs[i].value]);
                }
            } break;
            case 'S': { // setter 自定义属性的Setter方法
                type |= YYEncodingTypePropertyCustomSetter;
                if (attrs[i].value) {
                    _setter = NSSelectorFromString([NSString stringWithUTF8String:attrs[i].value]);
                }
            } // break; commented for code coverage in next line
            default: break;
        }
    }
    if (attrs) {
        free(attrs);
        attrs = NULL;
    }
    
    _type = type;
    
    if (_name.length) {
        if (!_getter) {
            // 如果属性getter方法，通过成员变量名生成对应的getter方法
            _getter = NSSelectorFromString(_name);
        }
        if (!_setter) {
            // 如果属性setter方法，通过成员变量名生成对应的setter方法
            _setter = NSSelectorFromString([NSString stringWithFormat:@"set%@%@:", [_name substringToIndex:1].uppercaseString, [_name substringFromIndex:1]]);
        }
    }
    return self;
}

@end

@implementation YYClassInfo {
    BOOL _needUpdate;
}

- (instancetype)initWithClass:(Class)cls {
    if (!cls) return nil;
    self = [super init];
    // 一般是模型类的类对象
    _cls = cls;
    // 一般是`NSObject`
    _superCls = class_getSuperclass(cls);
    _isMeta = class_isMetaClass(cls);
    
    if (!_isMeta) {
        _metaCls = objc_getMetaClass(class_getName(cls));
    }
    // 模型类的类名
    _name = NSStringFromClass(cls);
    
    // 更新当前实例的属性值，获取属性、成员、方法列表数据
    [self _update];
    _superClassInfo = [self.class classInfoWithClass:_superCls];
    return self;
}

- (void)_update {
    _ivarInfos = nil;
    _methodInfos = nil;
    _propertyInfos = nil;
    
    Class cls = self.cls;
    // 获取模型类的实例方法
    unsigned int methodCount = 0;
    
    /**
     获取模型类的实例方法
     如果想要获取类方法，传入`object_getClass(cls)`
     `class_copyMethodList`方法返回的是`Method*`类型，即数组首地址的指针，可以通过下标访问数组元素。
     */
    Method *methods = class_copyMethodList(cls, &methodCount);
    if (methods) {
        NSMutableDictionary *methodInfos = [NSMutableDictionary new];
        _methodInfos = methodInfos;
        for (unsigned int i = 0; i < methodCount; i++) {
            YYClassMethodInfo *info = [[YYClassMethodInfo alloc] initWithMethod:methods[i]];
            if (info.name) methodInfos[info.name] = info;
        }
        free(methods);
    }
    
    // 获取模型类的属性信息
    unsigned int propertyCount = 0;
    objc_property_t *properties = class_copyPropertyList(cls, &propertyCount);
    if (properties) {
        NSMutableDictionary *propertyInfos = [NSMutableDictionary new];
        _propertyInfos = propertyInfos;
        for (unsigned int i = 0; i < propertyCount; i++) {
            YYClassPropertyInfo *info = [[YYClassPropertyInfo alloc] initWithProperty:properties[i]];
            if (info.name) propertyInfos[info.name] = info;
        }
        free(properties);
    }
    
    // 获取模型类成员变量信息
    unsigned int ivarCount = 0;
    Ivar *ivars = class_copyIvarList(cls, &ivarCount);
    if (ivars) {
        NSMutableDictionary *ivarInfos = [NSMutableDictionary new];
        _ivarInfos = ivarInfos;
        for (unsigned int i = 0; i < ivarCount; i++) {
            YYClassIvarInfo *info = [[YYClassIvarInfo alloc] initWithIvar:ivars[i]];
            if (info.name) ivarInfos[info.name] = info;
        }
        free(ivars);
    }
    
    if (!_ivarInfos) _ivarInfos = @{};
    if (!_methodInfos) _methodInfos = @{};
    if (!_propertyInfos) _propertyInfos = @{};
    
    _needUpdate = NO;
}

- (void)setNeedUpdate {
    _needUpdate = YES;
}

- (BOOL)needUpdate {
    return _needUpdate;
}

+ (instancetype)classInfoWithClass:(Class)cls {
    if (!cls) return nil;
    static CFMutableDictionaryRef classCache;
    static CFMutableDictionaryRef metaCache;
    static dispatch_once_t onceToken;
    static dispatch_semaphore_t lock;
    dispatch_once(&onceToken, ^{
        classCache = CFDictionaryCreateMutable(CFAllocatorGetDefault(), 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        metaCache = CFDictionaryCreateMutable(CFAllocatorGetDefault(), 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        lock = dispatch_semaphore_create(1);
    });
    dispatch_semaphore_wait(lock, DISPATCH_TIME_FOREVER);
    YYClassInfo *info = CFDictionaryGetValue(class_isMetaClass(cls) ? metaCache : classCache, (__bridge const void *)(cls));
    if (info && info->_needUpdate) {
        [info _update];
    }
    dispatch_semaphore_signal(lock);
    if (!info) {
        info = [[YYClassInfo alloc] initWithClass:cls];
        if (info) {
            dispatch_semaphore_wait(lock, DISPATCH_TIME_FOREVER);
            CFDictionarySetValue(info.isMeta ? metaCache : classCache, (__bridge const void *)(cls), (__bridge const void *)(info));
            dispatch_semaphore_signal(lock);
        }
    }
    return info;
}

+ (instancetype)classInfoWithClassName:(NSString *)className {
    Class cls = NSClassFromString(className);
    return [self classInfoWithClass:cls];
}

@end
