/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <android/log.h>

#include <GLES/gl.h>
//#ifndef ANDROID_NDK
//#include <GLES/egl.h>
//#endif /* !ANDROID_NDK */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "IRenderingEngine.hpp"

#define  LOG_TAG    "NDKGL"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

IRenderingEngine* m_renderingEngine;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
		 = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

///// OpenGL 1 Stuff for export

extern "C" {
    JNIEXPORT void JNICALL Java_com_loft_ndkgl_OGLRenderer_nativeInit(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_loft_ndkgl_OGLRenderer_nativeResize(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_loft_ndkgl_OGLRenderer_nativeRender(JNIEnv * env, jobject obj);
	
    JNIEXPORT void JNICALL Java_com_loft_ndkgl_DemoGLSurfaceView_nativePause(JNIEnv * env, jobject obj);
	
	JNIEXPORT void JNICALL Java_com_loft_ndkgl_DemoActivity_nativeRotated(JNIEnv * env, jobject obj, jint deviceOrientation);
};

//JNIEXPORT void JNICALL Java_com_loft_ndkgl_DemoRenderer_nativeInit(JNIEnv * env, jobject obj)
JNIEXPORT void JNICALL Java_com_loft_ndkgl_OGLRenderer_nativeInit(JNIEnv * env, jobject obj)
{
//	LOGI("---- nativeInit ---- ES1");
//	m_renderingEngine = CreateRenderer1();
	
	LOGI("---- nativeInit ---- ES2");
	m_renderingEngine = CreateRenderer2();
	
	LOGI("---- Created Rendering Engine ----");
	
}

JNIEXPORT void JNICALL Java_com_loft_ndkgl_OGLRenderer_nativeResize(JNIEnv * env, jobject obj,  jint width, jint height){
	LOGI("---- nativeResize ----");
	m_renderingEngine->Initialize(width, height);
	LOGI("---- Inited Engine! ----");
}

JNIEXPORT void JNICALL Java_com_loft_ndkgl_OGLRenderer_nativeRender(JNIEnv * env, jobject obj){
	m_renderingEngine->Render();
}

JNIEXPORT void JNICALL Java_com_loft_ndkgl_DemoGLSurfaceView_nativePause(JNIEnv * env, jobject obj){
	LOGI("---- nativePause ----");
}

JNIEXPORT void JNICALL Java_com_loft_ndkgl_DemoActivity_nativeRotated(JNIEnv * env, jobject obj, jint deviceOrientation){
	int orientation;
	
	if (deviceOrientation < 45 || deviceOrientation >= 315) {
		orientation = 1;
	} else if (deviceOrientation >= 45 && deviceOrientation < 135) {
		orientation = 4;
	} else if (deviceOrientation >= 135 && deviceOrientation < 225) {
		orientation = 2;
	} else if (deviceOrientation >= 225 && deviceOrientation < 315) {
		orientation = 3;
	}

	LOGI("DEVICE ROTATED TO %d DEGREE => %d", deviceOrientation, orientation);
	m_renderingEngine->OnRotate((DeviceOrientation)orientation);

}

