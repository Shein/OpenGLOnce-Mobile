/*
 *  IRenderingEngine.hpp
 *  HelloArrow
 *
 *  Created by Daniel Shein on 9/30/10.
 *  Copyright 2010 LoFT. All rights reserved.
 *
 */

// Physical orientation of a handheld device, equivalent to UIDeviceOrientation
enum DeviceOrientation {
    DeviceOrientationUnknown = 0,
    DeviceOrientationPortrait = 1,
    DeviceOrientationPortraitUpsideDown = 2,
    DeviceOrientationLandscapeLeft = 3,
    DeviceOrientationLandscapeRight = 4,
    DeviceOrientationFaceUp = 5,
    DeviceOrientationFaceDown = 6,
};

// Create an instance of the renderer and set up various OpenGL state.
struct IRenderingEngine* CreateRenderer1();
struct IRenderingEngine* CreateRenderer2();

// Interface to the OpenGL ES renderer; consumed by GLView.
struct IRenderingEngine {
    virtual void Initialize(int width, int height) = 0;    
    virtual void Render() const = 0;
    virtual void UpdateAnimation(float timeStep) = 0;
    virtual void OnRotate(DeviceOrientation newOrientation) = 0;
    virtual ~IRenderingEngine() {}
};
