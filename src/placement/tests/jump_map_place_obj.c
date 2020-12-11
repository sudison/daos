/**
 * (C) Copyright 2016-2020 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * GOVERNMENT LICENSE RIGHTS-OPEN SOURCE SOFTWARE
 * The Government's rights to use, modify, reproduce, release, perform, display,
 * or disclose this software are subject to the terms of the Apache License as
 * provided in Contract No. B609815.
 * Any reproduction of computer software, computer software documentation, or
 * portions thereof marked with this legend must also reproduce the markings.
 */
#define D_LOGFAC	DD_FAC(tests)

#include <daos/common.h>
#include <daos/placement.h>
#include <daos.h>
#include "place_obj_common.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <daos/tests_lib.h>

static bool g_verbose;

#define skip_msg(msg) print_message("Skipping > "msg"\n"); skip()

void verbose_print(char *msg, ...)
{
	if (g_verbose) {
		va_list vargs;
		va_start(vargs, msg);
		vprint_message(msg, vargs);
		va_end(vargs);
	}
}

#define DOM_NR		18
#define	NODE_PER_DOM	1
#define VOS_PER_TARGET	4
#define SPARE_MAX_NUM	(DOM_NR * 3)
#define COMPONENT_NR	(DOM_NR + DOM_NR * NODE_PER_DOM + \
			 DOM_NR * NODE_PER_DOM * VOS_PER_TARGET)
#define NUM_TARGETS	(DOM_NR * NODE_PER_DOM * VOS_PER_TARGET)

#define TEST_PER_OC 10

static bool			 pl_debug_msg;

void
placement_object_class(daos_oclass_id_t cid)
{
	struct pool_map		*po_map;
	struct pl_map		*pl_map;
	struct pl_obj_layout	*layout;
	daos_obj_id_t		oid;
	int			test_num;

	gen_pool_and_placement_map(DOM_NR, NODE_PER_DOM,
				   VOS_PER_TARGET, PL_TYPE_JUMP_MAP,
				   &po_map, &pl_map);
	D_ASSERT(po_map != NULL);
	D_ASSERT(pl_map != NULL);

	srand(time(NULL));
	oid.hi = 5;

	for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
		oid.lo = rand();
		daos_obj_generate_id(&oid, 0, cid, 0);

		assert_success(plt_obj_place(oid, &layout, pl_map, false));
		plt_obj_layout_check(layout, COMPONENT_NR, 0);

		pl_obj_layout_free(layout);
	}

	free_pool_and_placement_map(po_map, pl_map);
	verbose_print("\tPlacement: OK\n");
}

void
rebuild_object_class(daos_oclass_id_t cid)
{
	struct pool_map		*po_map;
	struct pl_map		*pl_map;
	uint32_t		 spare_tgt_ranks[SPARE_MAX_NUM];
	uint32_t		 shard_ids[SPARE_MAX_NUM];
	daos_obj_id_t		 oid;
	uuid_t			 pl_uuid;
	struct daos_obj_md	 *md_arr;
	struct daos_obj_md	 md = { 0 };
	struct pl_obj_layout	**org_layout;
	struct pl_obj_layout	*layout;
	uint32_t		 po_ver;
	int			 test_num;
	int			 num_new_spares;
	int			 fail_tgt;
	int			 spares_left;
	int			 rc, i;

	uuid_generate(pl_uuid);
	srand(time(NULL));
	oid.hi = 5;
	po_ver = 1;

	D_ALLOC_ARRAY(md_arr, TEST_PER_OC);
	D_ASSERT(md_arr != NULL);
	D_ALLOC_ARRAY(org_layout, TEST_PER_OC);
	D_ASSERT(org_layout != NULL);

	gen_pool_and_placement_map(DOM_NR, NODE_PER_DOM,
				   VOS_PER_TARGET, PL_TYPE_JUMP_MAP,
				   &po_map, &pl_map);
	D_ASSERT(po_map != NULL);
	D_ASSERT(pl_map != NULL);

	/* Create array of object IDs to use later */
	for (i = 0; i < TEST_PER_OC; ++i) {
		oid.lo = rand();
		daos_obj_generate_id(&oid, 0, cid, 0);
		dc_obj_fetch_md(oid, &md);
		md.omd_ver = po_ver;
		md_arr[i] = md;
	}

	/* Generate layouts for later comparison */
	for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
		rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
				  &org_layout[test_num]);
		assert_success(rc);
		plt_obj_layout_check(org_layout[test_num], COMPONENT_NR, 0);
	}

	for (fail_tgt = 0; fail_tgt < NUM_TARGETS; ++fail_tgt) {
		/* Fail target and update the pool map */
		plt_fail_tgt(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);
		pl_map = pl_map_find(pl_uuid, oid);

		/*
		 * For each failed target regenerate all layouts and
		 * fetch rebuild targets, then verify basic conditions met
		 */
		for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
			md_arr[test_num].omd_ver = po_ver;

			rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
					  &layout);
			assert_success(rc);

			num_new_spares = pl_obj_find_rebuild(pl_map,
							     &md_arr[test_num], NULL, po_ver,
							     spare_tgt_ranks, shard_ids,
							     SPARE_MAX_NUM);

			spares_left = NUM_TARGETS - layout->ol_nr + fail_tgt;
			plt_obj_rebuild_layout_check(layout,
						     org_layout[test_num], COMPONENT_NR,
						     &fail_tgt, 1, spares_left,
						     num_new_spares, spare_tgt_ranks,
						     shard_ids);

			pl_obj_layout_free(layout);
		}

		/* Move target to Down state */
		plt_fail_tgt_out(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);
		pl_map = pl_map_find(pl_uuid, oid);

		/* Verify post rebuild layout */
		for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
			md_arr[test_num].omd_ver = po_ver;

			rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
					  &layout);
			assert_success(rc);
			D_ASSERT(layout->ol_nr == org_layout[test_num]->ol_nr);

			plt_obj_layout_check(layout, COMPONENT_NR,
					     layout->ol_nr);
			pl_obj_layout_free(layout);
		}

	}

	/* Cleanup Memory */
	for (i = 0; i < TEST_PER_OC; ++i)
		D_FREE(org_layout[i]);
	D_FREE(org_layout);
	free_pool_and_placement_map(po_map, pl_map);
	verbose_print("\tRebuild: OK\n");
}

void
reint_object_class(daos_oclass_id_t cid)
{
	struct pool_map		*po_map;
	struct pl_map		*pl_map;
	uint32_t		spare_tgt_ranks[SPARE_MAX_NUM];
	uint32_t		shard_ids[SPARE_MAX_NUM];
	daos_obj_id_t		oid;
	uuid_t			pl_uuid;
	struct daos_obj_md	*md_arr;
	struct daos_obj_md	md = { 0 };
	struct pl_obj_layout	***layout;
	struct pl_obj_layout	*temp_layout;
	uint32_t		po_ver;
	int			test_num;
	int			num_reint;
	int			fail_tgt;
	int			spares_left;
	int			rc, i;

	uuid_generate(pl_uuid);
	srand(time(NULL));
	oid.hi = 5;
	po_ver = 1;

	D_ALLOC_ARRAY(md_arr, TEST_PER_OC);
	D_ASSERT(md_arr != NULL);
	D_ALLOC_ARRAY(layout, NUM_TARGETS + 1);
	D_ASSERT(layout != NULL);

	for (i = 0; i < NUM_TARGETS + 1; ++i) {
		D_ALLOC_ARRAY(layout[i], TEST_PER_OC);
		D_ASSERT(layout[i] != NULL);
	}

	gen_pool_and_placement_map(DOM_NR, NODE_PER_DOM,
				   VOS_PER_TARGET, PL_TYPE_JUMP_MAP,
				   &po_map, &pl_map);
	D_ASSERT(po_map != NULL);
	D_ASSERT(pl_map != NULL);

	for (i = 0; i < TEST_PER_OC; ++i) {
		oid.lo = rand();
		daos_obj_generate_id(&oid, 0, cid, 0);
		dc_obj_fetch_md(oid, &md);
		md.omd_ver = po_ver;
		md_arr[i] = md;
	}

	/* Generate original layouts for later comparison*/
	for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
		rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
				  &layout[0][test_num]);
		assert_success(rc);
		plt_obj_layout_check(layout[0][test_num], COMPONENT_NR, 0);
	}

	/* fail all the targets */
	for (fail_tgt = 0; fail_tgt < NUM_TARGETS; ++fail_tgt) {

		plt_fail_tgt(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);

		plt_fail_tgt_out(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);

		/* Generate layouts for all N target failures*/
		for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
			md_arr[test_num].omd_ver = po_ver;

			rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
					  &layout[fail_tgt + 1][test_num]);
			assert_success(rc);
			plt_obj_layout_check(layout[fail_tgt + 1][test_num],
					     COMPONENT_NR, NUM_TARGETS);
		}
	}

	/* Reintegrate targets one-by-one and compare layouts */
	spares_left = NUM_TARGETS - (layout[0][0]->ol_nr + fail_tgt);
	for (fail_tgt = NUM_TARGETS-1; fail_tgt >= 0; --fail_tgt) {
		plt_reint_tgt(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);
		pl_map = pl_map_find(pl_uuid, oid);

		for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
			md_arr[test_num].omd_ver = po_ver;
			rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
					  &temp_layout);
			assert_success(rc);

			num_reint = pl_obj_find_reint(pl_map, &md_arr[test_num],
						      NULL, po_ver,  spare_tgt_ranks,
						      shard_ids, SPARE_MAX_NUM);

			plt_obj_reint_layout_check(temp_layout,
						   layout[fail_tgt][test_num],
						   COMPONENT_NR, &fail_tgt, 1, spares_left,
						   num_reint, spare_tgt_ranks, shard_ids);

			pl_obj_layout_free(temp_layout);
		}

		/* Set the target to up */
		plt_reint_tgt_up(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);

		/*
		 * Verify that the post-reintegration layout matches the
		 * pre-failure layout
		 */
		for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
			md_arr[test_num].omd_ver = po_ver;

			rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
					  &temp_layout);
			assert_success(rc);

			plt_obj_layout_check(temp_layout, COMPONENT_NR,
					     NUM_TARGETS);

			D_ASSERT(plt_obj_layout_match(temp_layout,
						      layout[fail_tgt][test_num]));

			pl_obj_layout_free(temp_layout);
		}

	}
	/* Cleanup Memory */
	for (i = 0; i <= NUM_TARGETS; ++i) {
		for (test_num = 0; test_num < TEST_PER_OC; ++test_num)
			D_FREE(layout[i][test_num]);
		D_FREE(layout[i]);
	}
	D_FREE(layout);
	free_pool_and_placement_map(po_map, pl_map);
	verbose_print("\tReintegration: OK\n");
}

void
drain_object_class(daos_oclass_id_t cid)
{
	struct pool_map		*po_map;
	struct pl_map		*pl_map;
	uint32_t		spare_tgt_ranks[SPARE_MAX_NUM];
	uint32_t		shard_ids[SPARE_MAX_NUM];
	uint32_t		po_ver;
	daos_obj_id_t		oid;
	uuid_t			pl_uuid;
	struct daos_obj_md	*md_arr;
	struct daos_obj_md	md = { 0 };
	struct pl_obj_layout	*layout;
	struct pl_obj_layout	**org_layout;
	int			test_num;
	int			num_new_spares;
	int			fail_tgt;
	int			rc, i;
	int			spares_left;

	uuid_generate(pl_uuid);
	srand(time(NULL));
	oid.hi = 5;
	po_ver = 1;

	D_ALLOC_ARRAY(md_arr, TEST_PER_OC);
	D_ASSERT(md_arr != NULL);
	D_ALLOC_ARRAY(org_layout, TEST_PER_OC);
	D_ASSERT(org_layout != NULL);

	gen_pool_and_placement_map(DOM_NR, NODE_PER_DOM,
				   VOS_PER_TARGET, PL_TYPE_JUMP_MAP,
				   &po_map, &pl_map);
	D_ASSERT(po_map != NULL);
	D_ASSERT(pl_map != NULL);

	for (i = 0; i < TEST_PER_OC; ++i) {
		oid.lo = rand();
		daos_obj_generate_id(&oid, 0, cid, 0);
		dc_obj_fetch_md(oid, &md);
		md.omd_ver = po_ver;
		md_arr[i] = md;
	}

	/* Generate layouts for later comparison*/
	for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
		md_arr[test_num].omd_ver = po_ver;

		rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
				  &org_layout[test_num]);
		assert_success(rc);
		plt_obj_layout_check(org_layout[test_num], COMPONENT_NR, 0);
	}

	for (fail_tgt = 0; fail_tgt < NUM_TARGETS; ++fail_tgt) {
		/* Drain target and update the pool map */
		plt_drain_tgt(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);
		pl_map = pl_map_find(pl_uuid, oid);

		spares_left = NUM_TARGETS - (org_layout[0]->ol_nr + fail_tgt);
		for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
			md_arr[test_num].omd_ver = po_ver;
			rc = pl_obj_place(pl_map, &md_arr[test_num],
					  NULL, &layout);
			assert_success(rc);

			num_new_spares = pl_obj_find_rebuild(pl_map,
							     &md_arr[test_num], NULL, po_ver,
							     spare_tgt_ranks, shard_ids,
							     SPARE_MAX_NUM);

			plt_obj_layout_check(layout, COMPONENT_NR,
					     layout->ol_nr);

			plt_obj_drain_layout_check(layout,
						   org_layout[test_num], COMPONENT_NR,
						   &fail_tgt, 1, spares_left,
						   num_new_spares, spare_tgt_ranks,
						   shard_ids);

			pl_obj_layout_free(layout);
		}

		/* Move target to Down-Out state */
		plt_fail_tgt_out(fail_tgt, &po_ver, po_map,  pl_debug_msg);
		pl_map_update(pl_uuid, po_map, false, PL_TYPE_JUMP_MAP);
		pl_map = pl_map_find(pl_uuid, oid);

		for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
			md_arr[test_num].omd_ver = po_ver;
			pl_obj_layout_free(org_layout[test_num]);

			rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
					  &org_layout[test_num]);
			assert_success(rc);

			plt_obj_layout_check(org_layout[test_num], COMPONENT_NR,
					     org_layout[test_num]->ol_nr);

		}

	}

	/* Cleanup Memory */
	for (i = 0; i < TEST_PER_OC; ++i)
		D_FREE(org_layout[i]);
	D_FREE(org_layout);
	free_pool_and_placement_map(po_map, pl_map);
	verbose_print("\tRebuild with Drain: OK\n");
}

#define NUM_TO_EXTEND 2

void
add_object_class(daos_oclass_id_t cid)
{
	struct pool_map		*po_map;
	struct pl_map		*pl_map;
	uint32_t		spare_tgt_ranks[SPARE_MAX_NUM];
	uint32_t		shard_ids[SPARE_MAX_NUM];
	uint32_t		po_ver;
	d_rank_list_t		rank_list;
	daos_obj_id_t		oid;
	uuid_t			pl_uuid;
	struct daos_obj_md	*md_arr;
	struct daos_obj_md	md = { 0 };
	struct pl_obj_layout	*layout;
	struct pl_obj_layout	**org_layout;
	int			test_num;
	int			num_new_spares;
	int			rc, i;
	uuid_t target_uuids[NUM_TO_EXTEND] = {"e0ab4def", "60fcd487"};
	int32_t domains[NUM_TO_EXTEND] = {1, 1};

	if (is_max_class_obj(cid))
		return;

	rank_list.rl_nr = NUM_TO_EXTEND;
	rank_list.rl_ranks = malloc(sizeof(d_rank_t) * NUM_TO_EXTEND);
	rank_list.rl_ranks[0] = DOM_NR + 1;
	rank_list.rl_ranks[1] = DOM_NR + 2;

	uuid_generate(pl_uuid);
	srand(time(NULL));
	oid.hi = 5;
	po_ver = 1;

	D_ALLOC_ARRAY(md_arr, TEST_PER_OC);
	D_ASSERT(md_arr != NULL);
	D_ALLOC_ARRAY(org_layout, TEST_PER_OC);
	D_ASSERT(org_layout != NULL);

	gen_pool_and_placement_map(DOM_NR, NODE_PER_DOM,
				   VOS_PER_TARGET, PL_TYPE_JUMP_MAP,
				   &po_map, &pl_map);
	D_ASSERT(po_map != NULL);
	D_ASSERT(pl_map != NULL);

	for (i = 0; i < TEST_PER_OC; ++i) {
		oid.lo = rand();
		daos_obj_generate_id(&oid, 0, cid, 0);
		dc_obj_fetch_md(oid, &md);
		md.omd_ver = po_ver;
		md_arr[i] = md;
	}

	/* Generate layouts for later comparison*/
	for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
		md_arr[test_num].omd_ver = po_ver;

		rc = pl_obj_place(pl_map, &md_arr[test_num], NULL,
				  &org_layout[test_num]);
		D_ASSERTF(rc == 0, "rc == %d\n", rc);
		plt_obj_layout_check(org_layout[test_num], COMPONENT_NR, 0);
	}
	extend_test_pool_map(po_map, NUM_TO_EXTEND, target_uuids, &rank_list,
			     NUM_TO_EXTEND, domains, NULL, NULL,
			     VOS_PER_TARGET);


	/* test normal placement for pools currently being extended */
	for (test_num = 0; test_num < TEST_PER_OC; ++test_num) {
		rc = pl_obj_place(pl_map, &md_arr[test_num], NULL, &layout);
		assert_success(rc);

		num_new_spares = pl_obj_find_addition(pl_map, &md_arr[test_num],
						      NULL, po_ver,
						      spare_tgt_ranks,
						      shard_ids,
						      SPARE_MAX_NUM);
		D_ASSERT(num_new_spares >= 0);

		plt_obj_add_layout_check(layout, org_layout[test_num],
					 COMPONENT_NR, num_new_spares,
					 spare_tgt_ranks, shard_ids);

		pl_obj_layout_free(layout);
	}

	/* Cleanup Memory */
	for (i = 0; i < TEST_PER_OC; ++i)
		D_FREE(org_layout[i]);
	D_FREE(org_layout);
	free_pool_and_placement_map(po_map, pl_map);
	verbose_print("\tAddition: OK\n");
}


static void
test_all_object_classes(void (*object_class_test)(daos_oclass_id_t))
{
	daos_oclass_id_t	*test_classes;
	uint32_t		 num_test_oc;
	char			 oclass_name[50];
	int			 oc_index;

	num_test_oc = get_object_classes(&test_classes);

	for (oc_index = 0; oc_index < num_test_oc; ++oc_index) {

		daos_oclass_id2name(test_classes[oc_index],  oclass_name);
		verbose_print("Running oclass test: %s\n", oclass_name);
		object_class_test(test_classes[oc_index]);
	}

	D_FREE(test_classes);
}

static void
placement_all_object_classes(void **state)
{
	test_all_object_classes(placement_object_class);
}

static void
rebuild_all_object_classes(void **state)
{
	test_all_object_classes(rebuild_object_class);
}

static void
reint_all_object_classes(void **state)
{
	test_all_object_classes(reint_object_class);
}

static void
drain_all_object_classes(void **state)
{
	test_all_object_classes(drain_object_class);
}

static void
add_all_object_classes(void **state)
{
	test_all_object_classes(add_object_class);
}

/* ---------------------------------------------------
 * New Tests
 * --------------------------------------------------- */

static void
gen_maps(int num_domains, int nodes_per_domain, int vos_per_target,
	 struct pool_map **po_map, struct pl_map **pl_map)
{
	*po_map = NULL;
	*pl_map = NULL;
	gen_pool_and_placement_map(num_domains, nodes_per_domain,
				   vos_per_target, PL_TYPE_JUMP_MAP,
				   po_map, pl_map);
	assert_non_null(*po_map);
	assert_non_null(*pl_map);
}

static void
gen_oid(daos_obj_id_t *oid, uint64_t lo, uint64_t hi, daos_oclass_id_t cid)
{
	oid->lo = lo;
	oid->hi = hi;
	daos_obj_generate_id(oid, 0, cid, 0);
}

#define assert_placement_success(pl_map, cid) \
	do {\
		daos_obj_id_t __oid; \
		struct pl_obj_layout *__layout = NULL; \
		gen_oid(&__oid, 1, UINT64_MAX, cid); \
		assert_success(plt_obj_place(__oid, &__layout, pl_map, \
				false)); \
		pl_obj_layout_free(__layout); \
	} while(0)
#define assert_invalid_param(pl_map, cid) \
	do {\
		daos_obj_id_t __oid; \
		struct pl_obj_layout *__layout = NULL; \
		gen_oid(&__oid, 1, UINT64_MAX, cid); \
		assert_err(plt_obj_place(__oid, &__layout, pl_map, \
				false), -DER_INVAL); \
	} while(0)

static void
object_class_is_verified(void **state)
{
	struct pool_map		*po_map;
	struct pl_map		*pl_map;
#define DAOS_6295 0
	/*
	 * ---------------------------------------------------------
	 * with a single target
	 * ---------------------------------------------------------
	 */
	gen_maps(1, 1, 1, &po_map, &pl_map);

	assert_invalid_param(pl_map, OC_UNKNOWN);
	assert_placement_success(pl_map, OC_S1);
	assert_placement_success(pl_map, OC_SX);

	/* Replication should fail because there's only 1 target */
	assert_invalid_param(pl_map, OC_RP_2G1);
	assert_invalid_param(pl_map, OC_RP_3G1);
	assert_invalid_param(pl_map, OC_RP_4G1);
	assert_invalid_param(pl_map, OC_RP_8G1);

#if DAOS_6295
	/* Multiple groups should fail because there's only 1 target */
	print_message("Skipping rest until DAOS-XXXX is resolved");
	assert_invalid_param(pl_map, OC_S2);
	assert_invalid_param(pl_map, OC_S4);
	assert_invalid_param(pl_map, OC_S512);
#endif
	free_pool_and_placement_map(po_map, pl_map);


	/*
	 * ---------------------------------------------------------
	 * with 2 targets
	 * ---------------------------------------------------------
	 */
	gen_maps(1, 1, 2, &po_map, &pl_map);

	assert_placement_success(pl_map, OC_S1);
	assert_placement_success(pl_map, OC_S2);
	assert_placement_success(pl_map, OC_SX);

	/*
	 * Even though there are 2 targets, these will still fail because
	 * placement requires a domain for each redundancy.
	 */
	assert_invalid_param(pl_map, OC_RP_2G1);
	assert_invalid_param(pl_map, OC_RP_2G2);
	assert_invalid_param(pl_map, OC_RP_3G1);
	assert_invalid_param(pl_map, OC_RP_4G1);
	assert_invalid_param(pl_map, OC_RP_8G1);
#if DAOS_6295
	/* The following require more targets than available. */
	assert_invalid_param(pl_map, OC_S4);
	assert_invalid_param(pl_map, OC_S512);
#endif
	free_pool_and_placement_map(po_map, pl_map);

	/*
	 * ---------------------------------------------------------
	 * With 2 domains, 1 target each
	 * ---------------------------------------------------------
	 */
	gen_maps(2, 1, 1, &po_map, &pl_map);

	assert_placement_success(pl_map, OC_S1);
	assert_placement_success(pl_map, OC_RP_2G1);
	assert_placement_success(pl_map, OC_RP_2GX);
#if DAOS_6295
	assert_invalid_param(pl_map, OC_RP_2G2);
	assert_invalid_param(pl_map, OC_RP_2G4);
#endif

	assert_invalid_param(pl_map, OC_RP_2G512);
	assert_invalid_param(pl_map, OC_RP_3G1);

	free_pool_and_placement_map(po_map, pl_map);

	/*
	 * ---------------------------------------------------------
	 * With 2 domains, 2 targets each = 4 targets
	 * ---------------------------------------------------------
	 */
	gen_maps(2, 1, 2, &po_map, &pl_map);
	assert_placement_success(pl_map, OC_RP_2G2);
#if DAOS_6295
	assert_invalid_param(pl_map, OC_RP_2G4);
#endif

	free_pool_and_placement_map(po_map, pl_map);

	/*
	 * ---------------------------------------------------------
	 * With 2 domains, 4 targets each = 8 targets
	 * ---------------------------------------------------------
	 */
	gen_maps(2, 1, 4, &po_map, &pl_map);
#if DAOS_6295
	assert_placement_success(pl_map, OC_RP_2G4);
#endif
	/* even though it's 8 total, still need a domain for each replica */
	assert_invalid_param(pl_map, OC_RP_4G2);

	free_pool_and_placement_map(po_map, pl_map);
	/*
	 * ---------------------------------------------------------
	 * With 2 domains, 2 nodes each, 2 targets each = 8 targets
	 * ---------------------------------------------------------
	 */
	gen_maps(2, 2, 2, &po_map, &pl_map);
	assert_placement_success(pl_map, OC_RP_2G4);
	/* even though it's 8 total, still need a domain for each replica */
	assert_invalid_param(pl_map, OC_RP_4G2);

	free_pool_and_placement_map(po_map, pl_map);

	/* The End */
	skip_msg("DAOS-6295: a bunch commented out in this test. ");
}

struct remap_result {
	uint32_t		*tgt_ranks;
	uint32_t		*ids;
	uint32_t		 nr;
	uint32_t		 out_nr;
};

static void rr_init(struct remap_result *rr, uint32_t nr)
{
	D_ALLOC_ARRAY(rr->ids, nr);
	D_ALLOC_ARRAY(rr->tgt_ranks, nr);
	rr->nr = nr;
	rr->out_nr = 0;
}

static void rr_fini(struct remap_result *rr)
{
	D_FREE(rr->ids);
	D_FREE(rr->tgt_ranks);
	memset(rr, 0, sizeof(*rr));
}

struct jm_test_ctx {
	struct pool_map		*po_map;
	struct pl_map		*pl_map;
	struct pl_obj_layout	*layout;
	struct remap_result	rebuild;
	struct remap_result	reint;
	struct remap_result	new;

	daos_obj_id_t		 oid;
	uint32_t 		 ver;
	uint32_t		 domain_nr;
	uint32_t		 node_nr;
	uint32_t		 target_nr;
	daos_oclass_id_t	 object_class;
	bool			 are_maps_generated;
	bool			 is_layout_set;
	bool			 enable_print_layout;
	bool			 enable_print_debug_msgs;
	bool			 enable_print_pool;
};

/* shard: struct pl_obj_shard * */
#define jtc_for_each_layout_shard(ctx, shard, i) \
	for (i = 0, shard = jtc_get_layout_shard(ctx, 0); \
		i < jtc_get_layout_shard_nr(ctx); \
		i++, shard = jtc_get_layout_shard(ctx, i))

static void
__jtc_maps_free(struct jm_test_ctx *ctx)
{
	if (ctx->are_maps_generated)
		free_pool_and_placement_map(ctx->po_map, ctx->pl_map);
}

static void
__jtc_inc_ver(struct jm_test_ctx *ctx)
{
	ctx->ver++;
}

static void
__jtc_layout_free(struct jm_test_ctx *ctx)
{
	if (ctx->is_layout_set)
		pl_obj_layout_free(ctx->layout);
}

static void
jtc_print_pool(struct jm_test_ctx *ctx)
{
	if (ctx->enable_print_pool)
		pool_map_print(ctx->po_map);
}

static void
jtc_print_pool_force(struct jm_test_ctx *ctx)
{
	pool_map_print(ctx->po_map);
}

static void
jtc_print_layout(struct jm_test_ctx *ctx)
{
	if (ctx->enable_print_layout)
		print_layout(ctx->layout);
}
static void
jtc_print_layout_force(struct jm_test_ctx *ctx)
{
	print_layout(ctx->layout);
}

static void
jtc_maps_gen(struct jm_test_ctx *ctx)
{
	/* Allocates the maps. must be freed with jtc_maps_free if already
	 * allocated
	 */
	__jtc_maps_free(ctx);

	gen_pool_and_placement_map(ctx->domain_nr, ctx->node_nr,
				   ctx->target_nr, PL_TYPE_JUMP_MAP,
				   &ctx->po_map, &ctx->pl_map);

	assert_non_null(ctx->po_map);
	assert_non_null(ctx->pl_map);
	ctx->are_maps_generated = true;
}

static void
jtc_scan(struct jm_test_ctx *ctx)
{
	struct daos_obj_md md = {.omd_id = ctx->oid, .omd_ver = ctx->ver};

	ctx->rebuild.out_nr = pl_obj_find_rebuild(ctx->pl_map, &md, NULL, ctx->ver,
						  ctx->rebuild.tgt_ranks,
						  ctx->rebuild.ids, ctx->rebuild.nr);
	ctx->reint.out_nr = pl_obj_find_reint(ctx->pl_map, &md, NULL, ctx->ver,
					      ctx->reint.tgt_ranks,
					      ctx->reint.ids, ctx->reint.nr);
	ctx->new.out_nr = pl_obj_find_rebuild(ctx->pl_map, &md, NULL, ctx->ver,
					      ctx->new.tgt_ranks,
					      ctx->new.ids, ctx->new.nr);
}

static int
jtc_create_layout(struct jm_test_ctx *ctx)
{
	int rc;

	/* place object will allocate the layout so need to free first
	 * if already allocated
	 */
	__jtc_layout_free(ctx);
	rc = plt_obj_place(ctx->oid, &ctx->layout, ctx->pl_map,
			   ctx->enable_print_layout);

	/* [todo-ryon]: Want to scan each time, but keep hitting
	 *  pl_map_common.c:remap_list_fill() ->  D_ASSERT(f_shard->fs_tgt_id != -1);
	 *
	 */
//	jtc_scan(ctx);

	ctx->is_layout_set = true;
	return rc;
}

static void
jtc_set_status_on_domain(struct jm_test_ctx *ctx, enum pool_comp_state status,
			 int id)
{
	plt_set_domain_status(id, status, &ctx->ver, ctx->po_map,
			      ctx->enable_print_debug_msgs, PO_COMP_TP_RACK);
	jtc_print_pool(ctx);
}

static int
__jtc_layout_tgt(struct jm_test_ctx *ctx, uint32_t shard_idx)
{
	return  ctx->layout->ol_shards[shard_idx].po_target;
}

static void
jtc_set_status_on_target(struct jm_test_ctx *ctx, const int status,
			 const uint32_t id)
{
	uuid_t uuid;

	__jtc_inc_ver(ctx);
	plt_set_tgt_status(id, status, ctx->ver, ctx->po_map,
			   ctx->enable_print_debug_msgs);

	pl_map_update(uuid, ctx->po_map, false, PL_TYPE_JUMP_MAP);
	ctx->pl_map = pl_map_find(uuid, ctx->oid);
	jtc_print_pool(ctx);
}

static void
jtc_set_status_on_shard_target(struct jm_test_ctx *ctx, const int status,
			       const uint32_t shard_idx)
{
	jtc_set_status_on_target(ctx, status, __jtc_layout_tgt(ctx, shard_idx));
}

static void
jtc_set_status_on_all_shards(struct jm_test_ctx *ctx, const int status)
{
	int i;

	for (i = 0; i < ctx->layout->ol_nr; i++)
		jtc_set_status_on_shard_target(ctx, status, i);
	jtc_print_pool(ctx);
}

static void
jtc_set_status_on_first_shard(struct jm_test_ctx *ctx, const int status)
{
	jtc_set_status_on_target(ctx, status, __jtc_layout_tgt(ctx, 0));
}

static void
jtc_reset_statuses(struct jm_test_ctx *ctx)
{
	int i;
	uint32_t target_nr = pool_map_target_nr(ctx->po_map);

	for (i = 0; i < target_nr; i++)
		jtc_set_status_on_target(ctx, PO_COMP_ST_UPIN, i);
	jtc_print_pool(ctx);

}

static void
jtc_set_object_meta(struct jm_test_ctx *ctx,
		    daos_oclass_id_t object_class, uint64_t lo, uint64_t hi)
{
	ctx->object_class = object_class;
	gen_oid(&ctx->oid, lo, hi, object_class);
}

static void
jtc_set_object_class(struct jm_test_ctx *ctx, daos_oclass_id_t object_class)
{
	jtc_set_object_meta(ctx, object_class, 1, UINT64_MAX);
}

static struct pl_obj_shard *
jtc_get_layout_shard(struct jm_test_ctx *ctx, const int shard_idx)
{
	if (shard_idx < ctx->layout->ol_nr)
		return &ctx->layout->ol_shards[shard_idx];

	return NULL;
}

static uint32_t
jtc_get_layout_shard_nr(struct jm_test_ctx *ctx)
{
	return ctx->layout->ol_nr;
}


static int
jtc_get_rebuild_count(struct jm_test_ctx *ctx)
{
	uint32_t result = 0;
	uint32_t i;
	struct pl_obj_shard *shard;

	jtc_for_each_layout_shard(ctx, shard, i) {
		if(shard->po_rebuilding)
			result ++;
	}

	return result;
}

static void
jtc_enable_debug(struct jm_test_ctx *ctx)
{
	ctx->enable_print_pool = true;
	ctx->enable_print_layout = true;
	ctx->enable_print_debug_msgs = true;
}
static void
jtc_init(struct jm_test_ctx *ctx, uint32_t domain_nr, uint32_t node_nr,
	 uint32_t target_nr, daos_oclass_id_t object_class, bool enable_debug)
{
	memset(ctx, 0, sizeof(*ctx));

	if (enable_debug)
		jtc_enable_debug(ctx);
	ctx->domain_nr = domain_nr;
	ctx->node_nr = node_nr;
	ctx->target_nr = target_nr;
	ctx->ver = 1; /* Should start with pool map version 1 */

	jtc_maps_gen(ctx);
	jtc_set_object_meta(ctx, object_class, 1, UINT64_MAX);

	/* hopefully this is enough */
	rr_init(&ctx->rebuild, ctx->domain_nr * 3);
	rr_init(&ctx->reint, ctx->domain_nr * 3);
	rr_init(&ctx->new, ctx->domain_nr * 3);
}

static void
jtc_init_with_layout(struct jm_test_ctx *ctx, uint32_t domain_nr,
		     uint32_t node_nr, uint32_t target_nr,
		     daos_oclass_id_t object_class, bool enable_debug)
{
	jtc_init(ctx, domain_nr, node_nr, target_nr, object_class,
		 enable_debug);
	jtc_create_layout(ctx);
}

static void
jtc_fini(struct jm_test_ctx *ctx)
{
	__jtc_layout_free(ctx);
	__jtc_maps_free(ctx);

	rr_fini(&ctx->rebuild);
	rr_fini(&ctx->reint);
	rr_fini(&ctx->new);
}

/* syntactical sugar to make tests easier to read */
enum shard_state {
	G = 1,
	B = 2,
	RB = 3,
};

#define DF_STATE "%s"
#define DP_STATE(s) s == G ? "G" : s == B ? "B" : "RB"


#define EXPECT_SHARD_STATES(ctx, ...) do {\
	uint16_t __expected[][2] = __VA_ARGS__;\
	expect_shard_states(__FILE__, __LINE__,  ctx, \
	__expected, ARRAY_SIZE(__expected)); } while (0)

static void
expect_shard_states(char *filename, uint32_t line_number, struct jm_test_ctx *ctx,
		    uint16_t expected_states[][2], uint32_t nr)
{
	int			 i;
	struct pl_obj_shard	*shard = NULL;

	assert_success(jtc_create_layout(ctx));
	jtc_print_layout(ctx);

	if (nr != jtc_get_layout_shard_nr(ctx))
		fail_msg("[%s:%d] Expected %d but found %d\n", filename, line_number, nr, jtc_get_layout_shard_nr(ctx));
	uint32_t e;
	enum shard_state actual_state;

	/* Make sure each actual shard is expected */
	jtc_for_each_layout_shard(ctx, shard, i) {
		bool found = false;
		actual_state = shard->po_target == -1 ? B :
			       shard->po_rebuilding ? RB :
			       G;
		for (e = 0; e < nr && found == false; e++) {
			if (shard->po_shard == expected_states[e][0] && actual_state == expected_states[e][1])
				found = true;
		}
		if (!found) {
			jtc_print_layout_force(ctx);
			fail_msg("[%s:%d] shard %d, state " DF_STATE" was not expected\n", filename, line_number, shard->po_shard, DP_STATE(actual_state));
		}
	}

	/* Make sure all expected is in layout */
	for (e = 0; e < nr; e++) {
		bool found = false;

		jtc_for_each_layout_shard(ctx, shard, i) {
			actual_state = shard->po_target == -1 ? B :
				       shard->po_rebuilding ? RB :
				       G;
			if (shard->po_shard == expected_states[e][0] && actual_state == expected_states[e][1])
				found = true;
		}
		if (!found) {
			jtc_print_layout_force(ctx);
			fail_msg("[%s:%d] expected %d, " DF_STATE" but not found.\n", filename, line_number, expected_states[e][0],
				 DP_STATE(expected_states[e][1]));
		}
	}
}

static void
upin_state(void **state)
{
	struct jm_test_ctx	 ctx;

	jtc_init(&ctx, 4, 1, 4, OC_RP_2G1, 0);
	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}});
	jtc_set_object_class(&ctx, OC_RP_2G2);
	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}, {2, G}, {3, G}});
	jtc_set_object_class(&ctx, OC_RP_2GX);

	EXPECT_SHARD_STATES(&ctx, {
		{ 0, G}, { 1, G}, { 2, G}, { 3, G},
		{ 4, G}, { 5, G}, { 6, G}, { 7, G},
		{ 8, G}, { 9, G}, {10, G}, {11, G},
		{12, G}, {13, G}, {14, G}, {15, G} });

	jtc_fini(&ctx);
}

static void
up_state(void **state)
{
	struct jm_test_ctx	 ctx;

	jtc_init(&ctx, 4, 1, 4, OC_RP_2G1, 0);

	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}});

	jtc_set_status_on_all_shards(&ctx, PO_COMP_ST_UP);
	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}, {0, RB}, {1, RB}});

	jtc_reset_statuses(&ctx);

	jtc_set_object_class(&ctx, OC_RP_2G2);
	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}, {2, G}, {3, G}});

	/* [todo-ryon]: Keep testing here */


	jtc_fini(&ctx);

}

/*
 * Test cases for targets in specific states
 */


/* DAOS-6297 */
static void
new_state(void **state)
{
	struct jm_test_ctx	 ctx;

	jtc_init_with_layout(&ctx, 2, 1, 4, OC_RP_2G1, 0);

	jtc_set_status_on_target(&ctx, PO_COMP_ST_NEW, 3);

	assert_success(jtc_create_layout(&ctx));

	jtc_set_status_on_target(&ctx, PO_COMP_ST_NEW, 7);
	assert_success(jtc_create_layout(&ctx));
	skip_msg("DAOS-6297: Not sure what's going on here or what to expect. With target "
		 "3 set as NEW (it would have been used as the placement for "
		 "shard 1) target 2 is chosen instead, but target 2 is not "
		 "included at all. But when target 7 is set as NEW (which is "
		 "not used in the layout at all) then target 2 and 3 are used "
		 "for shard 1, but neither are marked as rebuild. I'm still "
		 "not sure if I even have a valid configuration setup.");

	jtc_fini(&ctx);
}

/* DAOS-6297 */
static void
new_state2(void **state)
{
	struct jm_test_ctx	 ctx;

	jtc_init_with_layout(&ctx, 2, 2, 4, OC_RP_2G2, 0);

	jtc_set_status_on_domain(&ctx, PO_COMP_ST_NEW, 1);

	jtc_create_layout(&ctx);
	skip_msg("DAOS-6297");
	EXPECT_SHARD_STATES(&ctx, {
		{0, G}, {1, G}, {0, RB},
		{2, G}, {3, G}, {2, RB},
	});

	jtc_fini(&ctx);
}

static void
down_state(void **state)
{
	struct jm_test_ctx	 ctx;

	jtc_init_with_layout(&ctx, 4, 1, 4, OC_RP_2G1, false);

	/* the target for the first shard repeatedly fails.  */
	jtc_set_status_on_first_shard(&ctx, PO_COMP_ST_DOWN);
	EXPECT_SHARD_STATES(&ctx, {{0, RB}, {1, G}});

	jtc_set_status_on_first_shard(&ctx, PO_COMP_ST_DOWN);
	EXPECT_SHARD_STATES(&ctx, {{0, RB}, {1, G}});

	jtc_set_status_on_first_shard(&ctx, PO_COMP_ST_DOWN);
	EXPECT_SHARD_STATES(&ctx, {{0, RB}, {1, G}});

	jtc_set_status_on_first_shard(&ctx, PO_COMP_ST_DOWN);
	EXPECT_SHARD_STATES(&ctx, {{0, RB}, {1, G}});

	jtc_fini(&ctx);
}

static void
downout_state(void **state)
{
	struct jm_test_ctx	 ctx;

	ctx.enable_print_layout = true;
	ctx.enable_print_debug_msgs = true;
	jtc_init(&ctx, 4, 1, 4, OC_RP_2G1, 0);

	EXPECT_SHARD_STATES(&ctx, {0, G, 1, G});

	jtc_set_status_on_all_shards(&ctx, PO_COMP_ST_DOWNOUT);
	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}});

	jtc_set_status_on_all_shards(&ctx, PO_COMP_ST_DOWNOUT);

	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}});

	jtc_fini(&ctx);
}

/* DAOS-6300 */
static void
drain_state(void **state)
{
	struct jm_test_ctx	 ctx;

	jtc_init(&ctx, 4, 1, 4, OC_RP_2G1, false);

	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}});

	jtc_set_status_on_all_shards(&ctx, PO_COMP_ST_DRAIN);
	skip_msg("DAOS-6300: Not sure if this is a bug or not, but all are marked as rebuild?");
	EXPECT_SHARD_STATES(&ctx, {{0, G}, {1, G}, {0, RB}, {1, RB}});

	jtc_fini(&ctx);
}

/* DAOS-6301 */
static void
placement_handles_multiple_states(void **state)
{
	struct jm_test_ctx	 ctx;

	jtc_init_with_layout(&ctx, 3, 1, 8, OC_RP_2G1, false);
	jtc_set_status_on_shard_target(&ctx, PO_COMP_ST_DOWN, 0);
	jtc_set_status_on_shard_target(&ctx, PO_COMP_ST_UP, 1);
	jtc_set_status_on_domain(&ctx, PO_COMP_ST_NEW, 2);

	skip_msg("DAOS-6301");
	assert_success(jtc_create_layout(&ctx));

	jtc_fini(&ctx);
}

static void
get_rebuild(void **state)
{
	struct jm_test_ctx	 ctx;
	jtc_init_with_layout(&ctx, 4, 1, 2, OC_RP_2G2, false);
	jtc_set_status_on_domain(&ctx, PO_COMP_ST_DOWN, 0);
	jtc_set_status_on_target(&ctx, PO_COMP_ST_DOWN, 7);

	struct daos_obj_md md = {.omd_id = ctx.oid, .omd_ver = ctx.ver};
	uint32_t		 spare_tgt_ranks[SPARE_MAX_NUM] = {0};
	uint32_t		 shard_ids[SPARE_MAX_NUM] = {0};

	int rc = pl_obj_find_rebuild(ctx.pl_map, &md, NULL, ctx.ver, spare_tgt_ranks, shard_ids, SPARE_MAX_NUM);

//	int i;
//	for (i = 0; i < rc; i++) {
//		print_message("rebuild: %d -> %d\n", shard_ids[i], spare_tgt_ranks[i]);
//	}

	jtc_create_layout(&ctx);


	jtc_fini(&ctx);
}

/* DAOS-6302 */
static void
get_reint(void **state)
{
	struct jm_test_ctx	 ctx;
	jtc_init(&ctx, 4, 1, 2, OC_RP_2G2, false);
	jtc_create_layout(&ctx);
	jtc_set_status_on_domain(&ctx, PO_COMP_ST_UP, 0);
	jtc_set_status_on_target(&ctx, PO_COMP_ST_UP, 7);


	struct daos_obj_md md = {.omd_id = ctx.oid, .omd_ver = ctx.ver};
	uint32_t		 spare_tgt_ranks[SPARE_MAX_NUM] = {0};
	uint32_t		 shard_ids[SPARE_MAX_NUM] = {0};


	int rc = pl_obj_find_reint(ctx.pl_map, &md, NULL, ctx.ver, spare_tgt_ranks, shard_ids, SPARE_MAX_NUM);

//	int i;
//	for (i = 0; i < rc; i++) {
//		print_message("reinitializing: %d -> %d\n", shard_ids[i], spare_tgt_ranks[i]);
//	}

	jtc_create_layout(&ctx);
	struct pl_obj_shard *shard;
	uint32_t rebuilding = jtc_get_rebuild_count(&ctx);

	skip_msg("DAOS-6302");
	assert_int_equal(2, rebuilding);

	jtc_fini(&ctx);
}

/* DAOS-6303 */
static void
get_add(void **state)
{
	struct jm_test_ctx	 ctx;
	uint32_t		 spare_tgt_ranks[SPARE_MAX_NUM] = {0};
	uint32_t		 shard_ids[SPARE_MAX_NUM] = {0};

	jtc_init(&ctx, 4, 2, 2, OC_RP_2G2, false);

	jtc_create_layout(&ctx);
	jtc_set_status_on_domain(&ctx, PO_COMP_ST_NEW, 3);
	jtc_set_status_on_target(&ctx, PO_COMP_ST_NEW, 3);

	struct daos_obj_md md = {.omd_id = ctx.oid, .omd_ver = ctx.ver};

	int rc = pl_obj_find_addition(ctx.pl_map, &md, NULL, ctx.ver, spare_tgt_ranks, shard_ids, SPARE_MAX_NUM);
//	print_message("Found %d additions\n", rc);
//	int i;
//	for (i = 0; i < rc; i++) {
//		print_message("adding: %d -> %d\n", shard_ids[i], spare_tgt_ranks[i]);
//	}

	jtc_create_layout(&ctx);

	uint32_t rebuilding = jtc_get_rebuild_count(&ctx);

	skip_msg("DAOS-6303");
	assert_int_equal(2, rebuilding);

	jtc_fini(&ctx);
}

/*
 * ------------------------------------------------
 * End Test Cases
 * ------------------------------------------------
 */

static int
placement_test_setup(void **state)
{
	return pl_init();
}

static int
placement_test_teardown(void **state)
{
	pl_fini();

	return 0;
}

#define TEST(dsc, test) { dsc, test, placement_test_setup, placement_test_teardown }
static const struct CMUnitTest legacy_tests[] = {
	TEST("PLACEMENT01: Test placement on all object classes", placement_all_object_classes),
	TEST("PLACEMENT02: Test rebuild on all object classes", rebuild_all_object_classes),
	TEST("PLACEMENT03: Test reintegrate on all object classes", reint_all_object_classes),
	TEST("PLACEMENT04: Test drain on all object classes", drain_all_object_classes),
	TEST("PLACEMENT05: Test add on all object classes", add_all_object_classes),
};

/* [todo-ryon]: Tests ...
 *   - Need to make the system setup more interesting.
 */

static const struct CMUnitTest new_tests[] = {
	TEST("PLACEMENT01: object class is verified appropriately",
	     object_class_is_verified),

	TEST("1: UPIN", upin_state),
	TEST("2: UP", up_state),
	TEST("3: NEW", new_state),
	TEST("4: NEW 2", new_state2),
	TEST("5: Target for first shard continually goes to DOWN state and "
	     "never finishes rebuild.",
	     down_state),
	TEST("6: DOWNOUT", downout_state),
	TEST("WIP 7: DRAIN", drain_state),
	TEST("8: Placement can handle multiple states",
	     placement_handles_multiple_states),
	TEST("9: Rebuild", get_rebuild),
	TEST("10: Reinit", get_reint),
	TEST("11: Add", get_add),
};


int
placement_tests_run(bool verbose)
{
	int rc = 0;

	g_verbose = verbose;

	rc += cmocka_run_group_tests_name("New Placement Tests", new_tests,
					  NULL, NULL);
//	rc += cmocka_run_group_tests_name("Legacy Placement Tests", legacy_tests,
//					   NULL, NULL);


	return rc;
}
