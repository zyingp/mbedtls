//
//  ViewController.m
//  mbedtls
//
//  Created by ZengYingpei on 2018/3/18.
//  Copyright © 2018年 ZengYingpei. All rights reserved.
//

#import "ViewController.h"

#include "starter.h"

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (IBAction)onStartClicked:(id)sender
{
    startFuzz();
}

@end
