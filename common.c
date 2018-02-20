/*
 * Copyright (c) 2017 Rob Clark <rclark@redhat.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

static struct gbm gbm;

const struct gbm * init_gbm(int drm_fd, int w, int h)
{
	gbm.dev = gbm_create_device(drm_fd);

	gbm.surface = gbm_surface_create(gbm.dev, w, h,
			GBM_FORMAT_XRGB8888,
			GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

	if (!gbm.surface) {
		printf("failed to create gbm surface\n");
		return NULL;
	}

	gbm.width = w;
	gbm.height = h;

	return &gbm;
}

static int egl_set_config(struct egl *egl, const EGLint * config_attribs) {
	EGLint n;
	EGLConfig *configs;
	if (!eglGetConfigs(egl->display, NULL, 0, &n)) {
		printf("failed to query number of EGL configs\n");
		return -1;
	}
	configs = malloc(sizeof(EGLConfig)*n);
	if (!configs || !eglGetConfigs(egl->display, configs, n, &n)) {
		printf("failed to query configs\n");
		free(configs);
		return -1;
	}
	for (int i = 0; i < n; i++) {
		//#define showProp(x) { EGLint tmp = -1; eglGetConfigAttrib(egl->display, configs[i], x, &tmp); printf("configs[%d]: %s = %x\n", i, #x, tmp); }
		#define showProp(x)
		showProp(EGL_RED_SIZE);
		showProp(EGL_GREEN_SIZE);
		showProp(EGL_BLUE_SIZE);
		showProp(EGL_ALPHA_SIZE);
		showProp(EGL_ALPHA_MASK_SIZE);
		showProp(EGL_BIND_TO_TEXTURE_RGB);
		showProp(EGL_BIND_TO_TEXTURE_RGBA);
		showProp(EGL_BUFFER_SIZE);
		showProp(EGL_COLOR_BUFFER_TYPE);
		showProp(EGL_CONFIG_CAVEAT);
		showProp(EGL_CONFIG_ID);
		showProp(EGL_CONFORMANT);
		showProp(EGL_DEPTH_SIZE);
		showProp(EGL_LEVEL);
		showProp(EGL_LUMINANCE_SIZE);
		showProp(EGL_MAX_PBUFFER_WIDTH);
		showProp(EGL_MAX_PBUFFER_HEIGHT);
		showProp(EGL_MAX_PBUFFER_PIXELS);
		showProp(EGL_MAX_SWAP_INTERVAL);
		showProp(EGL_MIN_SWAP_INTERVAL);
		showProp(EGL_NATIVE_RENDERABLE);
		showProp(EGL_NATIVE_VISUAL_ID);
		showProp(EGL_NATIVE_VISUAL_TYPE);
		showProp(EGL_RENDERABLE_TYPE);
		showProp(EGL_SAMPLE_BUFFERS);
		showProp(EGL_SAMPLES);
		showProp(EGL_STENCIL_SIZE);
		showProp(EGL_SURFACE_TYPE);
		showProp(EGL_TRANSPARENT_TYPE);
		showProp(EGL_TRANSPARENT_RED_VALUE);
		showProp(EGL_TRANSPARENT_GREEN_VALUE);
		showProp(EGL_TRANSPARENT_BLUE_VALUE);
	}
	free(configs);
	if (!eglChooseConfig(egl->display, config_attribs, &egl->config, 1, &n) || n != 1) {
		printf("failed to choose config: %d\n", n);
		return -1;
	}
	return 0;
}

int init_egl(struct egl *egl, const struct gbm *gbm)
{
	EGLint major, minor;
	int ret;

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 1,
		EGL_NONE
	};

	static const EGLint config_attribs[] = {
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_NONE
	};

#define get_proc(name) do { \
		egl->name = (void *)eglGetProcAddress(#name); \
	} while (0)

	get_proc(eglGetPlatformDisplayEXT);
	get_proc(eglCreateImageKHR);
	get_proc(eglDestroyImageKHR);
	get_proc(glEGLImageTargetTexture2DOES);
	get_proc(eglCreateSyncKHR);
	get_proc(eglDestroySyncKHR);
	get_proc(eglWaitSyncKHR);
	get_proc(eglDupNativeFenceFDANDROID);

	if (egl->eglGetPlatformDisplayEXT) {
		egl->display = egl->eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_KHR,
				gbm->dev, NULL);
	} else {
		egl->display = eglGetDisplay((void *)gbm->dev);
	}

	if (!eglInitialize(egl->display, &major, &minor)) {
		printf("failed to initialize\n");
		return -1;
	}

	printf("Using display %p with EGL version %d.%d\n",
			egl->display, major, minor);

	printf("===================================\n");
	printf("EGL information:\n");
	printf("  version: \"%s\"\n", eglQueryString(egl->display, EGL_VERSION));
	printf("  vendor: \"%s\"\n", eglQueryString(egl->display, EGL_VENDOR));
	printf("  extensions: \"%s\"\n", eglQueryString(egl->display, EGL_EXTENSIONS));
	printf("===================================\n");

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		printf("failed to bind api EGL_OPENGL_ES_API\n");
		return -1;
	}

	ret = egl_set_config(egl, config_attribs);
	if (ret) {
		return ret;
	}

	egl->context = eglCreateContext(egl->display, egl->config,
			EGL_NO_CONTEXT, context_attribs);
	if (egl->context == NULL) {
		printf("failed to create context\n");
		return -1;
	}

	egl->surface = eglCreateWindowSurface(egl->display, egl->config,
			(EGLNativeWindowType)gbm->surface, NULL);
	if (egl->surface == EGL_NO_SURFACE) {
		printf("failed to create egl surface\n");
		return -1;
	}

	/* connect the context to the surface */
	eglMakeCurrent(egl->display, egl->surface, egl->surface, egl->context);

	printf("Context information:\n");
	printf("  version: \"%s\"\n", glGetString(GL_VERSION));
	//printf("  shading language version: \"%s\"\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("  vendor: \"%s\"\n", glGetString(GL_VENDOR));
	printf("  renderer: \"%s\"\n", glGetString(GL_RENDERER));
	printf("  extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));
	printf("===================================\n");

	return 0;
}
