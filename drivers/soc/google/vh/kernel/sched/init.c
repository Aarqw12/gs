// SPDX-License-Identifier: GPL-2.0-only
/* init.c
 *
 * Android Vendor Hook Support
 *
 * Copyright 2020 Google LLC
 */

#include <kernel/sched/sched.h>
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <trace/hooks/binder.h>
#include <trace/hooks/cgroup.h>
#include <trace/hooks/sched.h>
#include <trace/hooks/topology.h>
#include <trace/hooks/cpufreq.h>

#include "sched_priv.h"

extern void init_uclamp_stats(void);
extern int create_procfs_node(void);
#if IS_ENABLED(CONFIG_VH_SCHED) && IS_ENABLED(CONFIG_PIXEL_EM)
extern void vh_arch_set_freq_scale_pixel_mod(void *data,
					     const struct cpumask *cpus,
					     unsigned long freq,
					     unsigned long max,
					     unsigned long *scale);
#endif
extern void vh_set_sugov_sched_attr_pixel_mod(void *data, struct sched_attr *attr);
extern void rvh_set_iowait_pixel_mod(void *data, struct task_struct *p, struct rq *rq,
				     int *should_iowait_boost);
extern void rvh_select_task_rq_rt_pixel_mod(void *data, struct task_struct *p, int prev_cpu,
					    int sd_flag, int wake_flags, int *new_cpu);
extern void vh_scheduler_tick_pixel_mod(void *data, struct rq *rq);
extern void rvh_cpu_overutilized_pixel_mod(void *data, int cpu, int *overutilized);
extern void rvh_uclamp_eff_get_pixel_mod(void *data, struct task_struct *p,
					 enum uclamp_id clamp_id, struct uclamp_se *uclamp_max,
					 struct uclamp_se *uclamp_eff, int *ret);
#if !IS_ENABLED(CONFIG_USE_VENDOR_GROUP_UTIL)
extern void rvh_util_est_update_pixel_mod(void *data, struct cfs_rq *cfs_rq, struct task_struct *p,
					   bool task_sleep, int *ret);
extern void rvh_cpu_cgroup_online_pixel_mod(void *data, struct cgroup_subsys_state *css);
#endif
extern void rvh_post_init_entity_util_avg_pixel_mod(void *data, struct sched_entity *se);
extern void rvh_check_preempt_wakeup_pixel_mod(void *data, struct rq *rq, struct task_struct *p,
			bool *preempt, bool *nopreempt, int wake_flags, struct sched_entity *se,
			struct sched_entity *pse, int next_buddy_marked, unsigned int granularity);
extern void vh_sched_uclamp_validate_pixel_mod(void *data, struct task_struct *tsk,
					       const struct sched_attr *attr, bool user,
					       int *ret, bool *done);
extern void init_uclamp_stats(void);
extern void vh_dup_task_struct_pixel_mod(void *data, struct task_struct *tsk,
					 struct task_struct *orig);
extern void rvh_select_task_rq_fair_pixel_mod(void *data, struct task_struct *p, int prev_cpu,
					      int sd_flag, int wake_flags, int *target_cpu);
extern void init_vendor_group_data(void);
extern void rvh_update_rt_rq_load_avg_pixel_mod(void *data, u64 now, struct rq *rq,
						struct task_struct *p, int running);
extern void rvh_set_task_cpu_pixel_mod(void *data, struct task_struct *p, unsigned int new_cpu);
extern void rvh_enqueue_task_pixel_mod(void *data, struct rq *rq, struct task_struct *p, int flags);
extern void rvh_dequeue_task_pixel_mod(void *data, struct rq *rq, struct task_struct *p, int flags);
extern void rvh_enqueue_task_fair_pixel_mod(void *data, struct rq *rq, struct task_struct *p,
					    int flags);
extern void rvh_dequeue_task_fair_pixel_mod(void *data, struct rq *rq, struct task_struct *p,
					    int flags);
extern void vh_binder_set_priority_pixel_mod(void *data, struct binder_transaction *t,
	struct task_struct *task);
extern void vh_binder_restore_priority_pixel_mod(void *data, struct binder_transaction *t,
	struct task_struct *task);
extern void rvh_rtmutex_prepare_setprio_pixel_mod(void *data, struct task_struct *p,
	struct task_struct *pi_task);
extern void vh_dump_throttled_rt_tasks_mod(void *data, int cpu, u64 clock, ktime_t rt_period,
					   u64 rt_runtime, s64 rt_period_timer_expires);

#if IS_ENABLED(CONFIG_RVH_SCHED_LIB)
extern void rvh_sched_setaffinity_mod(void *data, struct task_struct *task,
					const struct cpumask *in_mask, int *res);
#endif /* IS_ENABLED(CONFIG_RVH_SCHED_LIB) */

extern void rvh_set_cpus_allowed_by_task(void *data, const struct cpumask *cpu_valid_mask,
	const struct cpumask *new_mask, struct task_struct *p, unsigned int *dest_cpu);
void sched_newidle_balance_pixel_mod(void *data, struct rq *this_rq, struct rq_flags *rf,
		int *pulled_task, int *done);
extern void rvh_can_migrate_task_pixel_mod(void *data, struct task_struct *p, int dst_cpu,
						int *can_migrate);
#if IS_ENABLED(CONFIG_USE_VENDOR_GROUP_UTIL)
extern void rvh_attach_entity_load_avg_pixel_mod(void *data, struct cfs_rq *cfs_rq,
						 struct sched_entity *se);
extern void rvh_detach_entity_load_avg_pixel_mod(void *data, struct cfs_rq *cfs_rq,
						 struct sched_entity *se);
extern void rvh_update_load_avg_pixel_mod(void *data, u64 now, struct cfs_rq *cfs_rq,
					  struct sched_entity *se);
extern void rvh_remove_entity_load_avg_pixel_mod(void *data, struct cfs_rq *cfs_rq,
						 struct sched_entity *se);
extern void rvh_update_blocked_fair_pixel_mod(void *data, struct rq *rq);
#endif
extern void android_vh_use_amu_fie_pixel_mod(void* data, bool *use_amu_fie);
extern void rvh_set_user_nice_pixel_mod(void *data, struct task_struct *p, long *nice,
					bool *allowed);
extern void rvh_setscheduler_pixel_mod(void *data, struct task_struct *p);
extern void rvh_find_lowest_rq_pixel_mod(void *data, struct task_struct *p,
					 struct cpumask *lowest_mask,
					 int ret, int *cpu);
extern void rvh_update_misfit_status_pixel_mod(void *data, struct task_struct *p,
			struct rq *rq, bool *need_update);

extern struct cpufreq_governor sched_pixel_gov;

extern int pmu_poll_init(void);

extern bool wait_for_init;

int pixel_cpu_num;
int pixel_cluster_num = 0;
int *pixel_cluster_start_cpu;
bool pixel_cpu_init = false;

EXPORT_SYMBOL_GPL(pixel_cpu_num);
EXPORT_SYMBOL_GPL(pixel_cluster_num);
EXPORT_SYMBOL_GPL(pixel_cluster_start_cpu);
EXPORT_SYMBOL_GPL(pixel_cpu_init);

/*
 * @tsk: Remote task we want to access its info
 * @saved_nice: Pointer to save the old nice value if we inherited a new one.
 * @prio_inherited: Returns whether we performed prio inheritance or not
 *
 * This function helps promote current prio to that of @tsk.
 *
 * We only do such for CFS tasks. Used to handle priority inversion when
 * holding mmap_sem in GKI.
 *
 * Returns true when inheritance was performed and saved_nice was updated.
 * False if no inheritance was necessary.
 */
static void vh_prio_inheritance(void *data, struct task_struct *tsk,
				int *saved_nice, bool *prio_inherited)
{
	int current_nice = task_nice(current);
	int target_nice;

	*prio_inherited = false;

	if (tsk == current)
		return;

	if (dl_task(current) || rt_task(current))
		return;

	if (dl_task(tsk) || rt_task(tsk))
		target_nice = 0;
	else
		target_nice = task_nice(tsk);

	/* Only promote, don't demote */
	if (current_nice <= target_nice)
		return;

	*saved_nice = current_nice;
	set_user_nice(current, target_nice);

	*prio_inherited = true;
}

static void vh_prio_restore(void *data, int nice)
{
	set_user_nice(current, nice);
}

static void init_vendor_rt_rq(void)
{
	int i;
	struct vendor_rq_struct *vrq;

	for (i = 0; i < pixel_cpu_num; i++) {
		vrq = get_vendor_rq_struct(cpu_rq(i));
		raw_spin_lock_init(&vrq->lock);
		vrq->util_removed = 0;
		atomic_set(&vrq->num_adpf_tasks, 0);
	}
}

static int init_vendor_task_data(void *data)
{
	struct vendor_task_struct *v_tsk;
	struct task_struct *p, *t;

	rcu_read_lock();
	for_each_process_thread(p, t) {
		get_task_struct(t);
		v_tsk = get_vendor_task_struct(t);
		init_vendor_task_struct(v_tsk);
		v_tsk->orig_prio = t->static_prio;
		put_task_struct(t);
	}
	rcu_read_unlock();

	/* our module can start handling the initialization now */
	wait_for_init = false;

	return 0;
}

static int init_pixel_cpu(void)
{
	int i, j = 0;
	unsigned long cur_capacity = 0;

	pixel_cpu_num = cpumask_weight(cpu_possible_mask);

	if (!pixel_cpu_num)
		return -EPROBE_DEFER;

	for_each_possible_cpu(i) {
		if (arch_scale_cpu_capacity(i) > cur_capacity) {
			cur_capacity = arch_scale_cpu_capacity(i);
			pixel_cluster_num++;
		}
	}

	pixel_cluster_start_cpu = kcalloc(pixel_cluster_num, sizeof(int), GFP_KERNEL);

	if (!pixel_cluster_start_cpu)
		return -ENOMEM;

	cur_capacity = 0;
	for_each_possible_cpu(i) {
		if (arch_scale_cpu_capacity(i) > cur_capacity) {
			pixel_cluster_start_cpu[j++] = i;
			cur_capacity = arch_scale_cpu_capacity(i);
		}
	}

	pixel_cpu_init = true;

	return 0;
}

static int vh_sched_init(void)
{
	int ret;

	ret = init_pixel_cpu();
	if (ret) {
		pr_err("pixel cpu init failed\n");
		return ret;
	}

	ret = pmu_poll_init();
	if (ret) {
		pr_err("pmu poll init failed\n");
		return ret;
	}

#if IS_ENABLED(CONFIG_UCLAMP_STATS)
	init_uclamp_stats();
#endif

	ret = create_procfs_node();
	if (ret)
		return ret;

	init_vendor_rt_rq();

	init_vendor_group_data();

	/*
	 * We must register this first but it won't do anything until we
	 * initialize vendor task data for all currently running tasks.
	 *
	 * We can't call this directly in init_vendor_task_data() as it'll hold
	 * a mutex and the context in stop_machine is atomic.
	 *
	 * init_vendor_task_data() should set a flag to enable this function to
	 * work as soon as we have initialized the task data.
	 */
	ret = register_trace_android_vh_dup_task_struct(vh_dup_task_struct_pixel_mod, NULL);
	if (ret)
		return ret;

	/*
	 * Heavy handed, but necessary. We want to initialize our private data
	 * structure for every task running in the system now. And register
	 * a hook to ensure we initialize them for future ones via
	 * dup_task_struct() vh.
	 *
	 * stop_machine provides atomic way to guarantee this without races.
	 */
	ret = stop_machine(init_vendor_task_data, NULL, cpumask_of(raw_smp_processor_id()));
	if (ret)
		return ret;

	ret = register_trace_android_rvh_enqueue_task(rvh_enqueue_task_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_dequeue_task(rvh_dequeue_task_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_can_migrate_task(rvh_can_migrate_task_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_enqueue_task_fair(rvh_enqueue_task_fair_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_dequeue_task_fair(rvh_dequeue_task_fair_pixel_mod, NULL);
	if (ret)
		return ret;

#if IS_ENABLED(CONFIG_USE_VENDOR_GROUP_UTIL)
	ret = register_trace_android_rvh_attach_entity_load_avg(
		rvh_attach_entity_load_avg_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_detach_entity_load_avg(
		rvh_detach_entity_load_avg_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_update_load_avg(rvh_update_load_avg_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_remove_entity_load_avg(
		rvh_remove_entity_load_avg_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_update_blocked_fair(
		rvh_update_blocked_fair_pixel_mod, NULL);
	if (ret)
		return ret;
#endif

	ret = register_trace_android_rvh_rtmutex_prepare_setprio(
		rvh_rtmutex_prepare_setprio_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_update_rt_rq_load_avg(rvh_update_rt_rq_load_avg_pixel_mod,
							       NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_set_task_cpu(rvh_set_task_cpu_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_set_iowait(rvh_set_iowait_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_select_task_rq_rt(rvh_select_task_rq_rt_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_vh_scheduler_tick(vh_scheduler_tick_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_cpu_overutilized(rvh_cpu_overutilized_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_uclamp_eff_get(rvh_uclamp_eff_get_pixel_mod, NULL);
	if (ret)
		return ret;

#if !IS_ENABLED(CONFIG_USE_VENDOR_GROUP_UTIL)
	ret = register_trace_android_rvh_util_est_update(rvh_util_est_update_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_cpu_cgroup_online(
		rvh_cpu_cgroup_online_pixel_mod, NULL);
	if (ret)
		return ret;
#endif

	ret = register_trace_android_rvh_sched_newidle_balance(
		sched_newidle_balance_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_update_misfit_status(
		rvh_update_misfit_status_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_post_init_entity_util_avg(
		rvh_post_init_entity_util_avg_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_check_preempt_wakeup(
		rvh_check_preempt_wakeup_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_select_task_rq_fair(rvh_select_task_rq_fair_pixel_mod,
							     NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_set_cpus_allowed_by_task(
		rvh_set_cpus_allowed_by_task, NULL);
	if (ret)
		return ret;

#if IS_ENABLED(CONFIG_VH_SCHED) && IS_ENABLED(CONFIG_PIXEL_EM)
	ret = register_trace_android_vh_arch_set_freq_scale(vh_arch_set_freq_scale_pixel_mod, NULL);
	if (ret)
		return ret;
#endif

	ret = register_trace_android_vh_uclamp_validate(
		vh_sched_uclamp_validate_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = cpufreq_register_governor(&sched_pixel_gov);
	if (ret)
		return ret;

	ret = register_trace_android_vh_dump_throttled_rt_tasks(vh_dump_throttled_rt_tasks_mod,
								NULL);
	if (ret)
		return ret;

#if IS_ENABLED(CONFIG_RVH_SCHED_LIB)

	ret = register_trace_android_rvh_sched_setaffinity(rvh_sched_setaffinity_mod, NULL);
	if (ret)
		return ret;
#endif /* IS_ENABLED(CONFIG_RVH_SCHED_LIB) */

	ret = register_trace_android_vh_binder_set_priority(
		vh_binder_set_priority_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_vh_binder_restore_priority(
		vh_binder_restore_priority_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_vh_use_amu_fie(android_vh_use_amu_fie_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_set_user_nice(rvh_set_user_nice_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_setscheduler(rvh_setscheduler_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_rvh_find_lowest_rq(rvh_find_lowest_rq_pixel_mod, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_vh_prio_inheritance(vh_prio_inheritance, NULL);
	if (ret)
		return ret;

	ret = register_trace_android_vh_prio_restore(vh_prio_restore, NULL);
	if (ret)
		return ret;

	// Disable TTWU_QUEUE.
	sysctl_sched_features &= ~(1UL << __SCHED_FEAT_TTWU_QUEUE);
	static_key_disable(&sched_feat_keys[__SCHED_FEAT_TTWU_QUEUE]);

	return 0;
}

module_init(vh_sched_init);
MODULE_LICENSE("GPL v2");
