//
//  NSObject+YYModel.h
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 15/5/10.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 提供了一些数据模型相互转换的方法
 
 * 把json转换成对象，或者把对象转换成json
 * 设置对象属性用键值对字典
 * 实现了 `NSCoding`, `NSCopying`, `-hash` 和 `-isEqual:`.
 
 See `YYModel` 协议中包含自定义方法
 
 示例代码:
    
     ********************** json 转换 *********************
 @code
     @interface YYAuthor : NSObject
     @property (nonatomic, strong) NSString *name;
     @property (nonatomic, assign) NSDate *birthday;
     @end
     @implementation YYAuthor
     @end
 
     @interface YYBook : NSObject
     @property (nonatomic, copy) NSString *name;
     @property (nonatomic, assign) NSUInteger pages;
     @property (nonatomic, strong) YYAuthor *author;
     @end
     @implementation YYBook
     @end
    
     int main() {
         // 通过json数据创建模型对象
         YYBook *book = [YYBook modelWithJSON:@"{\"name\": \"Harry Potter\", \"pages\": 256, \"author\": {\"name\": \"J.K.Rowling\", \"birthday\": \"1965-07-31\" }}"];
 
         // 把模型对象转换成json
         NSString *json = [book modelToJSONString];
         // {"author":{"name":"J.K.Rowling","birthday":"1965-07-31T00:00:00+0000"},"name":"Harry Potter","pages":256}
     }
 @endcode
 
 
     ********************** 协议相关 *********************
 @code
     @interface YYShadow :NSObject <NSCoding, NSCopying>
     @property (nonatomic, copy) NSString *name;
     @property (nonatomic, assign) CGSize size;
     @end
 
     @implementation YYShadow
     - (void)encodeWithCoder:(NSCoder *)aCoder { [self modelEncodeWithCoder:aCoder]; }
     - (id)initWithCoder:(NSCoder *)aDecoder { self = [super init]; return [self modelInitWithCoder:aDecoder]; }
     - (id)copyWithZone:(NSZone *)zone { return [self modelCopy]; }
     - (NSUInteger)hash { return [self modelHash]; }
     - (BOOL)isEqual:(id)object { return [self modelIsEqual:object]; }
     @end
 @endcode
 
 */
@interface NSObject (YYModel)

/**
 通过json创建并返回一个调用该方法类的实例对象，该方法线程安全。
 
 @param json  `NSDictionary`, `NSString` 或 `NSData`类型的json对象
 
 @return 通过json构建的实例
 */
+ (nullable instancetype)modelWithJSON:(id)json;

/**
 通过键值字典创建返回一个调用者类型的实例对象
 该方法是线程安全的
 
 @param dictionary  映射到调用类的属性的字典
 字典中无效的键值对会被忽略
 
 @return 通过字典创建的实例对象
 
 @discussion 字典中的键会被映射到调用类的属性名，键对应的值会被设置给对应的属性。如果值类型和类对应属性
 的类型不匹配，该方法将会根据以下规则进行转换：
    `NSString` or `NSNumber` -> c number, 比如： BOOL, int, long, float, NSUInteger...
    `NSString` -> NSDate, 用 "yyyy-MM-dd'T'HH:mm:ssZ", "yyyy-MM-dd HH:mm:ss" 或 "yyyy-MM-dd" 格式进行解析
    `NSString` -> NSURL.
    `NSValue` -> 结构体或联合体, 比如 CGRect, CGSize, ...
    `NSString` -> SEL, Class.
 */
+ (nullable instancetype)modelWithDictionary:(NSDictionary *)dictionary;

/**
 用json对象给调用类的属性赋值
 
 @discussion json中无效的数据会被忽略
 
 @param json  `NSDictionary`, `NSString` 或 `NSData`类型的json对象, 映射到调用类的属性
 
 @return 是否成功
 */
- (BOOL)modelSetWithJSON:(id)json;

/**
 用键值对字典给调用类的属性赋值
 
 @param dic  键值对字典，映射调用类的属性，字典中无效的键值对会被忽略
 
 @discussion 字典中的键映射到调用类的属性名，字典中的值会赋值给对应的属性值。如果字典中值的类型和类中属性类型不匹配
 该方法将会按以下规则进行转换：
     `NSString`, `NSNumber` -> c number, such as BOOL, int, long, float, NSUInteger...
     `NSString` -> NSDate, parsed with format "yyyy-MM-dd'T'HH:mm:ssZ", "yyyy-MM-dd HH:mm:ss" or "yyyy-MM-dd".
     `NSString` -> NSURL.
     `NSValue` -> struct or union, such as CGRect, CGSize, ...
     `NSString` -> SEL, Class.
 
 @return 是否成功
 */
- (BOOL)modelSetWithDictionary:(NSDictionary *)dic;

/**
 通过调用类的属性生成json对象
 
 @return `NSDictionary` 或 `NSArray`类型的json对象 A json object in , or nil if an error occurs.
 See [NSJSONSerialization isValidJSONObject] 可以获取更多信息
 
 @discussion 无效的属性会被忽略，如果调用该方法的类是`NSArray`, `NSDictionary` 或 `NSSet`，这个方法只会把内部对象转换成
 json对象
 */
- (nullable id)modelToJSONObject;

/**
 从调用类的属性生成一个json字符串的数据
 
 @return json字符串数据
 
 @discussion 调用类中无效的属性会被忽略
 `NSArray`, `NSDictionary` 或 `NSSet`对象调用该方法, 该方法还会将内部对象转为json字符串数据
 */
- (nullable NSData *)modelToJSONData;

/**
 通过调用类的属性生成json字符串.
 
 @return json字符串.
 
 @discussion 调用类中无效的属性会被忽略，如果调用类是`NSArray`, `NSDictionary` 或 `NSSet`类型，内部的对象也会
 被转换成json字符串
 */
- (nullable NSString *)modelToJSONString;

/**
 用调用类的属性复制一个对象
 
 @return 被拷贝的对象
 */
- (nullable id)modelCopy;

/**
 将调用类的属性编码到编码器中.
 
 @param aCoder  归档对象.
 */
- (void)modelEncodeWithCoder:(NSCoder *)aCoder;

/**
通过传入的解码器解码调用类的属性
 
 @param aDecoder  归档对象.
 
 @return 当前类
 */
- (id)modelInitWithCoder:(NSCoder *)aDecoder;

/**
 获取调用类属性的哈希值
 
 @return 哈希值.
 */
- (NSUInteger)modelHash;

/**
 Compares the receiver with another object for equality, based on properties.
 
 @param model  Another object.
 
 @return `YES` if the reciever is equal to the object, otherwise `NO`.
 */
- (BOOL)modelIsEqual:(id)model;

/**
 基于属性用于调试的描述方法.
 
 @return 描述调用类内容的字符串.
 */
- (NSString *)modelDescription;

@end



/**
 给数组类型添加的数据模型方法
 */
@interface NSArray (YYModel)

/**
 从json数组创建并返回一个数组，该方法线程安全。
 
 @param cls  数组中实例的类型
 @param json  json数组，比如: [{"name":"Mary"},{name:"Joe"}]
 
 @return 数组
 */
+ (nullable NSArray *)modelArrayWithClass:(Class)cls json:(id)json;

@end



/**
 给字典类型添加的数据模型方法
 */
@interface NSDictionary (YYModel)

/**
 通过json创建并返回一个字典，该方法线程安全。
 
 @param cls  字典中值实例的类
 @param json  json字典，比如: {"user1":{"name","Mary"}, "user2": {name:"Joe"}}
 
 @return 字典
 */
+ (nullable NSDictionary *)modelDictionaryWithClass:(Class)cls json:(id)json;
@end



/**
 If the default model transform does not fit to your model class, implement one or
 more method in this protocol to change the default key-value transform process.
 There's no need to add '<YYModel>' to your class header.
 */
@protocol YYModel <NSObject>
@optional

/**
 自定义属性映射关系
 
 @discussion 如果在数据json或字典中没有找到和模型属性名匹配的key，实现这个方法返回带有映射关系的映射器
 
 例子:
    
    json: 
        {
            "n":"Harry Pottery",
            "p": 256,
            "ext" : {
                "desc" : "A book written by J.K.Rowling."
            },
            "ID" : 100010
        }
 
    model:
    @code
        @interface YYBook : NSObject
        @property NSString *name;
        @property NSInteger page;
        @property NSString *desc;
        @property NSString *bookID;
        @end
        
        @implementation YYBook
        + (NSDictionary *)modelCustomPropertyMapper {
            return @{@"name"  : @"n",
                     @"page"  : @"p",
                     @"desc"  : @"ext.desc", // 将数据中ext内部的desc字段映射到数据模型desc属性中
                     @"bookID": @[@"id", @"ID", @"book_id"]}; // 数据中的id、ID、book_id都会被映射到YYBook模型的bookID属性中
        @end
    @endcode
 
 @return 一个用于属性的自定义映射器
 */
+ (nullable NSDictionary<NSString *, id> *)modelCustomPropertyMapper;

/**
 容器属性的泛型类映射器
 
 @discussion 如果属性的类型是NSArray/NSSet/NSDictionary对象， 实现这个方法返回一个 属性->类 的映射器
 通过这个方法描述哪种对象可以被添加到array/set/dictionary中。
 
  例子:
  @code
        @class YYShadow, YYBorder, YYAttachment;
 
        @interface YYAttributes
        @property NSString *name;
        @property NSArray *shadows;
        @property NSSet *borders;
        @property NSDictionary *attachments;
        @end
 
        @implementation YYAttributes
        + (NSDictionary *)modelContainerPropertyGenericClass {
            return @{@"shadows" : [YYShadow class],
                     @"borders" : YYBorder.class,
                     @"attachments" : @"YYAttachment" };
        }
        @end
 @endcode
 
 @return 类映射器
 */
+ (nullable NSDictionary<NSString *, id> *)modelContainerPropertyGenericClass;

/**
 如果需要在数据转模型过程中创建不同类的实例，可以使用这个方法通过字典数据选择生成不同类型的模型类
 
 @discussion 如果模型类实现了这个方法，该方法会在`+modelWithJSON:`, `+modelWithDictionary:`方法中被调用用来确定最终转换成的模型
 类，转换父对象属性的对象，单个还是容器都通过`+modelContainerPropertyGenericClass`
  
 Example:
 @code
        @class YYCircle, YYRectangle, YYLine;
 
        @implementation YYShape

        + (Class)modelCustomClassForDictionary:(NSDictionary*)dictionary {
            if (dictionary[@"radius"] != nil) {
                return [YYCircle class]; // 如果数据中`radius`不为空，模型类转为YYCircle
            } else if (dictionary[@"width"] != nil) {
                return [YYRectangle class]; // 如果数据中`width`不为空，模型类转为YYRectangle
            } else if (dictionary[@"y2"] != nil) {
                return [YYLine class]; // 如果数据中`y2`不为空，模型类转为YYLine
            } else {
                return [self class]; // 如果都为空，则转为当前类
            }
        }

        @end
 @endcode

 @param dictionary json或键值对字典
 
 @return 根据数据字典返回的类，如果返回nil则使用当前类

 */
+ (nullable Class)modelCustomClassForDictionary:(NSDictionary *)dictionary;

/**
 黑名单中的属性在数据转模型过程中将被忽略（不转换）
 
 @return 包含属性名称的数组
 */
+ (nullable NSArray<NSString *> *)modelPropertyBlacklist;

/**
 添加到白名单的属性将会被转换，没有加入白名单的将会被忽略。
 
 @return 包含属性名称的数组
 */
+ (nullable NSArray<NSString *> *)modelPropertyWhitelist;

/**
 该方法类似于`- (BOOL)modelCustomTransformFromDictionary:(NSDictionary *)dic;`
 区别在于该方法会在模型转换之前被调用
 
 @discussion 如果模型类实现了这个方法，则在执行这几个方法：`+modelWithJSON:`, `+modelWithDictionary:`, `-modelSetWithJSON:` 和 `-modelSetWithDictionary:`之前调用这个方法，如果返回nil，则转换过程将忽略这个模型。
 
 @param dic  json或键值对字典
 
 @return 返回修改后的字典
 */
- (NSDictionary *)modelCustomWillTransformFromDictionary:(NSDictionary *)dic;

/**
 如果默认的数据转模型不适合模型类，实现这个方法去添加额外的处理，也可以通过这个方法验证转换后的模型的属性转换情况。
 
 @discussion 这个方法会在`+modelWithJSON:`, `+modelWithDictionary:`, `-modelSetWithJSON:` 和 `-modelSetWithDictionary:`之后被调用，如果该方法返回NO,则转换过程中会忽略这个模型。
 
 @param dic  json或键值对字典
 
 @return 返回YES代表该方法生效，返回NO则忽略
 */
- (BOOL)modelCustomTransformFromDictionary:(NSDictionary *)dic;

/**
 如果默认的模型转json不适合模型类，实现这个方法去添加额外的处理，也可以通过这个方法验证转换后的json字典
 
 @discussion 这个方法会在`-modelToJSONObject` and `-modelToJSONString`执行后被调用，如果返回NO则转换过程将忽略这个json字典
 
 @param dic json字典
 
 @return 返回YES代表该方法生效，返回NO则忽略
 */
- (BOOL)modelCustomTransformToDictionary:(NSMutableDictionary *)dic;

@end

NS_ASSUME_NONNULL_END
