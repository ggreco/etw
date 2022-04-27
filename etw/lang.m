
#import <Foundation/Foundation.h>

const char *get_lang_id()
{
    NSString * language = [[NSLocale preferredLanguages] objectAtIndex:0];

    return [language UTF8String];
}