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
#include "esUtil.h"


struct {
	struct egl egl;

	GLfloat aspect;
} gl;

static const GLfloat vVertices[] = {
		// front
		-1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		// back
		+1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, -1.0f,
		// right
		+1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f,
		// left
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, +1.0f,
		// top
		-1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		// bottom
		-1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f,
};

static const GLfloat vColors[] = {
		// front
		0.0f,  0.0f,  1.0f,  1.0f, // blue
		1.0f,  0.0f,  1.0f,  1.0f, // magenta
		0.0f,  1.0f,  1.0f,  1.0f, // cyan
		1.0f,  1.0f,  1.0f,  1.0f, // white
		// back
		1.0f,  0.0f,  0.0f,  1.0f, // red
		0.0f,  0.0f,  0.0f,  1.0f, // black
		1.0f,  1.0f,  0.0f,  1.0f, // yellow
		0.0f,  1.0f,  0.0f,  1.0f, // green
		// right
		1.0f,  0.0f,  1.0f,  1.0f, // magenta
		1.0f,  0.0f,  0.0f,  1.0f, // red
		1.0f,  1.0f,  1.0f,  1.0f, // white
		1.0f,  1.0f,  0.0f,  1.0f, // yellow
		// left
		0.0f,  0.0f,  0.0f,  1.0f, // black
		0.0f,  0.0f,  1.0f,  1.0f, // blue
		0.0f,  1.0f,  0.0f,  1.0f, // green
		0.0f,  1.0f,  1.0f,  1.0f, // cyan
		// top
		0.0f,  1.0f,  1.0f,  1.0f, // cyan
		1.0f,  1.0f,  1.0f,  1.0f, // white
		0.0f,  1.0f,  0.0f,  1.0f, // green
		1.0f,  1.0f,  0.0f,  1.0f, // yellow
		// bottom
		0.0f,  0.0f,  0.0f,  1.0f, // black
		1.0f,  0.0f,  0.0f,  1.0f, // red
		0.0f,  0.0f,  1.0f,  1.0f, // blue
		1.0f,  0.0f,  1.0f,  1.0f, // magenta
};

static const GLfloat vNormals[] = {
		// front
		+0.0f, +0.0f, +1.0f, // forward
		+0.0f, +0.0f, +1.0f, // forward
		+0.0f, +0.0f, +1.0f, // forward
		+0.0f, +0.0f, +1.0f, // forward
		// back
		+0.0f, +0.0f, -1.0f, // backward
		+0.0f, +0.0f, -1.0f, // backward
		+0.0f, +0.0f, -1.0f, // backward
		+0.0f, +0.0f, -1.0f, // backward
		// right
		+1.0f, +0.0f, +0.0f, // right
		+1.0f, +0.0f, +0.0f, // right
		+1.0f, +0.0f, +0.0f, // right
		+1.0f, +0.0f, +0.0f, // right
		// left
		-1.0f, +0.0f, +0.0f, // left
		-1.0f, +0.0f, +0.0f, // left
		-1.0f, +0.0f, +0.0f, // left
		-1.0f, +0.0f, +0.0f, // left
		// top
		+0.0f, +1.0f, +0.0f, // up
		+0.0f, +1.0f, +0.0f, // up
		+0.0f, +1.0f, +0.0f, // up
		+0.0f, +1.0f, +0.0f, // up
		// bottom
		+0.0f, -1.0f, +0.0f, // down
		+0.0f, -1.0f, +0.0f, // down
		+0.0f, -1.0f, +0.0f, // down
		+0.0f, -1.0f, +0.0f  // down
};

static void draw_cube_smooth(unsigned i)
{
	ESMatrix modelview;

	/* clear the color buffer */
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	esMatrixLoadIdentity(&modelview);
	esTranslate(&modelview, 0.0f, 0.0f, -8.0f);
	esRotate(&modelview, 45.0f + (0.25f * i), 1.0f, 0.0f, 0.0f);
	esRotate(&modelview, 45.0f - (0.5f * i), 0.0f, 1.0f, 0.0f);
	esRotate(&modelview, 10.0f + (0.15f * i), 0.0f, 0.0f, 1.0f);

	ESMatrix projection;
	esMatrixLoadIdentity(&projection);
	esFrustum(&projection, -2.8f, +2.8f, -2.8f * gl.aspect, +2.8f * gl.aspect, 6.0f, 10.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat *)modelview.m);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat *)projection.m);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
}

const struct egl * init_cube_smooth(const struct gbm *gbm)
{
	int ret;

	ret = init_egl(&gl.egl, gbm);
	if (ret)
		return NULL;

	gl.aspect = (GLfloat)(gbm->height) / (GLfloat)(gbm->width);

	glViewport(0, 0, gbm->width, gbm->height);
	glEnable(GL_CULL_FACE);

	glVertexPointer(3, GL_FLOAT, 0, vVertices);
	glColorPointer(4, GL_FLOAT, 0, vColors);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	gl.egl.draw = draw_cube_smooth;

	return &gl.egl;
}
