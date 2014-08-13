/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdlib.h>
#include <kernel/tee_core_trace.h>
#include <utee_defines.h>
#include <kernel/tee_time.h>

struct tee_ta_time_offs {
	TEE_UUID uuid;
	TEE_Time offs;
	bool positive;
};

static struct tee_ta_time_offs *tee_time_offs;
static size_t tee_time_num_offs;

static TEE_Result tee_time_ta_get_offs(const TEE_UUID *uuid,
				       const TEE_Time **offs, bool *positive)
{
	size_t n;

	for (n = 0; n < tee_time_num_offs; n++) {
		if (memcmp(uuid, &tee_time_offs[n].uuid, sizeof(TEE_UUID))
				== 0) {
			*offs = &tee_time_offs[n].offs;
			*positive = tee_time_offs[n].positive;
			return TEE_SUCCESS;
		}
	}
	return TEE_ERROR_TIME_NOT_SET;
}

static TEE_Result tee_time_ta_set_offs(const TEE_UUID *uuid,
				       const TEE_Time *offs, bool positive)
{
	size_t n;
	struct tee_ta_time_offs *o;

	for (n = 0; n < tee_time_num_offs; n++) {
		if (memcmp(uuid, &tee_time_offs[n].uuid, sizeof(TEE_UUID))
				== 0) {
			tee_time_offs[n].offs = *offs;
			tee_time_offs[n].positive = positive;
			return TEE_SUCCESS;
		}
	}

	n = tee_time_num_offs + 1;
	o = malloc(n * sizeof(struct tee_ta_time_offs));
	if (o == NULL)
		return TEE_ERROR_OUT_OF_MEMORY;
	memcpy(o, tee_time_offs,
	       tee_time_num_offs * sizeof(struct tee_ta_time_offs));
	free(tee_time_offs);
	tee_time_offs = o;
	tee_time_offs[tee_time_num_offs].uuid = *uuid;
	tee_time_offs[tee_time_num_offs].offs = *offs;
	tee_time_offs[tee_time_num_offs].positive = positive;
	tee_time_num_offs = n;
	return TEE_SUCCESS;
}

TEE_Result tee_time_get_ta_time(const TEE_UUID *uuid, TEE_Time *time)
{
	TEE_Result res;
	const TEE_Time *offs;
	bool positive;
	TEE_Time t;
	TEE_Time t2;

	res = tee_time_ta_get_offs(uuid, &offs, &positive);
	if (res != TEE_SUCCESS)
		return res;

	res = tee_time_get_sys_time(&t);
	if (res != TEE_SUCCESS)
		return res;

	if (positive) {
		TEE_TIME_ADD(t, *offs, t2);

		/* Detect wrapping, the wrapped time should be returned. */
		if (TEE_TIME_LT(t2, t))
			res = TEE_ERROR_OVERFLOW;
	} else {
		TEE_TIME_SUB(t, *offs, t2);

		/* Detect wrapping, the wrapped time should be returned. */
		if (TEE_TIME_LE(t, t2))
			res = TEE_ERROR_OVERFLOW;
	}
	*time = t2;

	return res;
}

TEE_Result tee_time_set_ta_time(const TEE_UUID *uuid, const TEE_Time *time)
{
	TEE_Result res;
	TEE_Time offs;
	TEE_Time t;

	/* Check that time is normalized. */
	if (time->millis >= TEE_TIME_MILLIS_BASE)
		return TEE_ERROR_BAD_PARAMETERS;

	res = tee_time_get_sys_time(&t);
	if (res != TEE_SUCCESS)
		return res;

	if (TEE_TIME_LT(t, *time)) {
		TEE_TIME_SUB(*time, t, offs);
		return tee_time_ta_set_offs(uuid, &offs, true);
	} else {
		TEE_TIME_SUB(t, *time, offs);
		return tee_time_ta_set_offs(uuid, &offs, false);
	}
}
