//
//  FileReq.m
//  etw
//
//  Created by Andrea Carolfi on 12/12/15.
//  Copyright © 2015 Gabriele Greco. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

#include "freq.h"

int MacRequester(struct MyFileRequest *fr)
{
    int result = 0;
    NSString *est,*dir;
    if(fr->Filter)
    {
        char *temp = strrchr(fr->Filter, '.')+1;
        est = [NSString stringWithCString:temp encoding:NSASCIIStringEncoding];
    }
    else
        est = @"";
    
    if(fr->Dir)
        dir = [NSString stringWithCString:fr->Dir encoding:NSASCIIStringEncoding];
    else
        dir = nil;
    
    if(!fr->Save)
    {
        //NSArray *fileTypes = [NSArray arrayWithObject:est];
        //NSArray *fileTypes = [NSArray arrayWithObject:@"car"];
        NSOpenPanel *oPanel = [NSOpenPanel openPanel];
        
        [oPanel setAllowsMultipleSelection:NO];
        /*choice = [oPanel runModalForDirectory:NSHomeDirectory()
         file:nil types:fileTypes];*/

        if ([oPanel runModal] == NSModalResponseOK) {
            NSURL*  aFile = [[oPanel URLs] objectAtIndex:0];
            fr->File = (char *)[[aFile path] cStringUsingEncoding:[NSString defaultCStringEncoding]];
            result = 1;
        }
    }
    else
    {
        /* create or get the shared instance of NSSavePanel */
        NSSavePanel *sp = [NSSavePanel savePanel];
        
        fr->Save = FALSE;
        
        /* set up new attributes */
        //[sp setAccessoryView:newView];
        sp.allowedFileTypes = [NSArray arrayWithObject:est];
        //[sp setRequiredFileType:@"car"];
        
        /* display the NSSavePanel */
        //choice = [sp runModal];
        
        /* if successful, save file under designated name */
        if ([sp runModal] == NSModalResponseOK) {
            NSURL*  aFile = [sp URL];
            fr->File = (char *)[[aFile path] cStringUsingEncoding:[NSString defaultCStringEncoding]];
            result = 1;
        }
    }
    return result;
}

const char *GetAppName(void)
{
    NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
    //NSString *appName = [[NSFileManager defaultManager] displayNameAtPath: bundlePath];
    return [bundlePath cStringUsingEncoding:[NSString defaultCStringEncoding]];
}
