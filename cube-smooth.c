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

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
//#include "esUtil.h"

#ifndef max
#define max(a, b) ((a)>(b)?(a):(b))
#endif

struct {
	struct egl egl;

	GLfloat aspect;
} gl;

static const GLfloat vVertices[] = {
		100.0f, 100.0f,
		400.0f, 100.0f,
		100.0f, 400.0f,
		400.0f, 400.0f,

		102.0f, 102.0f,
		398.0f, 102.0f,
		102.0f, 125.0f,
		398.0f, 125.0f,
};

static const GLfloat vColors[] = {
		0.75f,0.75f,0.75f,1.0f,
		0.75f,0.75f,0.75f,1.0f,
		0.75f,0.75f,0.75f,1.0f,
		0.75f,0.75f,0.75f,1.0f,
		0.00f,0.10f,0.80f,1.0f,
		0.00f,0.10f,0.50f,1.0f,
		0.00f,0.10f,0.80f,1.0f,
		0.00f,0.10f,0.50f,1.0f,
};

static void draw_cube_smooth(unsigned i)
{
	(void)i;
	/* clear the color buffer */
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
}

const struct egl * init_cube_smooth(const struct gbm *gbm)
{
	int ret;

	ret = init_egl(&gl.egl, gbm);
	if (ret)
		return NULL;

	gl.aspect = (GLfloat)(gbm->height) / (GLfloat)(gbm->width);

	glViewport(0, 0, gbm->width, gbm->height);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, gbm->width, gbm->height, 0, 1, -1);

	glVertexPointer(2, GL_FLOAT, 0, vVertices);
	glColorPointer(4, GL_FLOAT, 0, vColors);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	{
		GLenum error = glGetError();
		while (error) {
			printf("GL Error during init: %d\n", error);
			return NULL;
		}
	}

	gl.egl.draw = draw_cube_smooth;

	return &gl.egl;
}
