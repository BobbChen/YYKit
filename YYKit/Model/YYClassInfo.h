//
//  YYClassInfo.h
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 15/5/9.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

NS_ASSUME_NONNULL_BEGIN

/**
 类型的复合编码，包含限定符、类型以及属性
 */
typedef NS_OPTIONS(NSUInteger, YYEncodingType) {
    YYEncodingTypeMask       = 0xFF, ///< 类型掩码
    YYEncodingTypeUnknown    = 0, ///< unknown
    YYEncodingTypeVoid       = 1, ///< void
    YYEncodingTypeBool       = 2, ///< bool
    YYEncodingTypeInt8       = 3, ///< char / BOOL
    YYEncodingTypeUInt8      = 4, ///< unsigned char
    YYEncodingTypeInt16      = 5, ///< short
    YYEncodingTypeUInt16     = 6, ///< unsigned short
    YYEncodingTypeInt32      = 7, ///< int
    YYEncodingTypeUInt32     = 8, ///< unsigned int
    YYEncodingTypeInt64      = 9, ///< long long
    YYEncodingTypeUInt64     = 10, ///< unsigned long long
    YYEncodingTypeFloat      = 11, ///< float
    YYEncodingTypeDouble     = 12, ///< double
    YYEncodingTypeLongDouble = 13, ///< long double
    YYEncodingTypeObject     = 14, ///< id
    YYEncodingTypeClass      = 15, ///< Class
    YYEncodingTypeSEL        = 16, ///< SEL
    YYEncodingTypeBlock      = 17, ///< block
    YYEncodingTypePointer    = 18, ///< void*
    YYEncodingTypeStruct     = 19, ///< struct
    YYEncodingTypeUnion      = 20, ///< union
    YYEncodingTypeCString    = 21, ///< char*
    YYEncodingTypeCArray     = 22, ///< char[10] (for example)
    
    YYEncodingTypeQualifierMask   = 0xFF00,   ///< 限定符掩码
    YYEncodingTypeQualifierConst  = 1 << 8,  ///< const
    YYEncodingTypeQualifierIn     = 1 << 9,  ///< in
    YYEncodingTypeQualifierInout  = 1 << 10, ///< inout
    YYEncodingTypeQualifierOut    = 1 << 11, ///< out
    YYEncodingTypeQualifierBycopy = 1 << 12, ///< bycopy
    YYEncodingTypeQualifierByref  = 1 << 13, ///< byref
    YYEncodingTypeQualifierOneway = 1 << 14, ///< oneway
    
    YYEncodingTypePropertyMask         = 0xFF0000, ///< 属性掩码
    YYEncodingTypePropertyReadonly     = 1 << 16, ///< readonly
    YYEncodingTypePropertyCopy         = 1 << 17, ///< copy
    YYEncodingTypePropertyRetain       = 1 << 18, ///< retain
    YYEncodingTypePropertyNonatomic    = 1 << 19, ///< nonatomic
    YYEncodingTypePropertyWeak         = 1 << 20, ///< weak
    YYEncodingTypePropertyCustomGetter = 1 << 21, ///< getter=
    YYEncodingTypePropertyCustomSetter = 1 << 22, ///< setter=
    YYEncodingTypePropertyDynamic      = 1 << 23, ///< @dynamic
};

/**
 通过类型编码字符串获取类型格式
 
 @discussion See also:
 https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/ObjCRuntimeGuide/Articles/ocrtTypeEncodings.html
 https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/ObjCRuntimeGuide/Articles/ocrtPropertyIntrospection.html
 
 @param typeEncoding  类型编码字符串
 @return 编码类型
 */
YYEncodingType YYEncodingGetType(const char *typeEncoding);


/**
 类成员变量信息
 */
@interface YYClassIvarInfo : NSObject
@property (nonatomic, assign, readonly) Ivar ivar;              ///< 不透明的成员变量结构体
@property (nonatomic, strong, readonly) NSString *name;         ///< 成员变量名称
@property (nonatomic, assign, readonly) ptrdiff_t offset;       ///< 成员变量的偏移
@property (nonatomic, strong, readonly) NSString *typeEncoding; ///< 成员变量的类型编码
@property (nonatomic, assign, readonly) YYEncodingType type;    ///< 成员变量的类型

/**
 构建一个含有成员变量信息的对象
 
 @param ivar ivar opaque struct
 @return A new object, or nil if an error occurs.
 */
- (instancetype)initWithIvar:(Ivar)ivar;
@end


/**
 类方法信息
 */
@interface YYClassMethodInfo : NSObject
@property (nonatomic, assign, readonly) Method method;                  ///< 原始Method
@property (nonatomic, strong, readonly) NSString *name;                 ///< 方法名
@property (nonatomic, assign, readonly) SEL sel;                        ///< 方法选择器
@property (nonatomic, assign, readonly) IMP imp;                        ///< 方法实现的首地址
@property (nonatomic, strong, readonly) NSString *typeEncoding;         ///< 方法的类型编码（参数+返回）
@property (nonatomic, strong, readonly) NSString *returnTypeEncoding;   ///< 方法返回值的类型编码
@property (nullable, nonatomic, strong, readonly) NSArray<NSString *> *argumentTypeEncodings; ///< 包含方法参数的类型编码的数组

/**
 创建并返回包含方法信息的对象
 
 @param method 不透明类型`Method`
 @return A new object, or nil if an error occurs.
 */
- (instancetype)initWithMethod:(Method)method;
@end


/**
 属性信息
 */
@interface YYClassPropertyInfo : NSObject
@property (nonatomic, assign, readonly) objc_property_t property; ///< 属性的不透明类型
@property (nonatomic, strong, readonly) NSString *name;           ///< 属性名
@property (nonatomic, assign, readonly) YYEncodingType type;      ///< 属性类型
@property (nonatomic, strong, readonly) NSString *typeEncoding;   ///< 属性的类型编码
@property (nonatomic, strong, readonly) NSString *ivarName;       ///< 属性的成员变量名称
@property (nullable, nonatomic, assign, readonly) Class cls;      ///< may be nil
@property (nullable, nonatomic, strong, readonly) NSArray<NSString *> *protocols; ///< may nil
@property (nonatomic, assign, readonly) SEL getter;               ///< 属性getter方法 (非空)
@property (nonatomic, assign, readonly) SEL setter;               ///< 属性的setter方法 (非空)

/**
 创建并返回包含属性信息的对象
 
 @param property property opaque struct
 @return A new object, or nil if an error occurs.
 */
- (instancetype)initWithProperty:(objc_property_t)property;
@end


/**
 类信息
 */
@interface YYClassInfo : NSObject
@property (nonatomic, assign, readonly) Class cls; ///< 类对象
@property (nullable, nonatomic, assign, readonly) Class superCls; ///< 父类对象
@property (nullable, nonatomic, assign, readonly) Class metaCls;  ///< 类的元类对象
@property (nonatomic, readonly) BOOL isMeta; ///< 当前类是否是元类
@property (nonatomic, strong, readonly) NSString *name; ///< 当前类雷鸣
@property (nullable, nonatomic, strong, readonly) YYClassInfo *superClassInfo; ///< 父类的类信息
@property (nullable, nonatomic, strong, readonly) NSDictionary<NSString *, YYClassIvarInfo *> *ivarInfos; ///< 成员变量信息字典
@property (nullable, nonatomic, strong, readonly) NSDictionary<NSString *, YYClassMethodInfo *> *methodInfos; ///< 方法信息字典
@property (nullable, nonatomic, strong, readonly) NSDictionary<NSString *, YYClassPropertyInfo *> *propertyInfos; ///< 属性信息字典

/**
 如果通过runtime修改了类比如通过`class_addMethod()`给类添加了方法，则需要调用该方法刷新类信息缓存。
 
 调用这个方法之后，`needUpdate` 会返回 `YES`，并且应该调用
 
 After called this method, `needUpdate` will returns `YES`, and you should call 
 'classInfoWithClass' or 'classInfoWithClassName' to get the updated class info.
 */
- (void)setNeedUpdate;

/**
 如果该方法返回`YES`, 则不应该再使用当前实例对象，需要调用`classInfoWithClass` 或 `classInfoWithClassName`来获取更新类信息之后的对象
 
 @return 类信息是否需要更新
 */
- (BOOL)needUpdate;

/**
 Get the class info of a specified Class.
 
 @discussion This method will cache the class info and super-class info
 at the first access to the Class. This method is thread-safe.
 
 @param cls A class.
 @return A class info, or nil if an error occurs.
 */
+ (nullable instancetype)classInfoWithClass:(Class)cls;

/**
 Get the class info of a specified Class.
 
 @discussion This method will cache the class info and super-class info
 at the first access to the Class. This method is thread-safe.
 
 @param className A class name.
 @return A class info, or nil if an error occurs.
 */
+ (nullable instancetype)classInfoWithClassName:(NSString *)className;

@end

NS_ASSUME_NONNULL_END
